#include "core.hpp"
#include "type_utils.hpp"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include <Luau/Compiler.h>
#include "common/Namecall_atom.hpp"
#include "lua_base.hpp"
#include "co_tasks.hpp"
#include "common/constext.hpp"
#include "common.hpp"
#include "library.hpp"
#include <stdexcept>
#include "library.hpp"
#include <string_view>
#include <variant>
#include <functional>
#include <format>
namespace fs = std::filesystem;
static constexpr auto builtin_name = "std";
static std::unique_ptr<lua_State, std::function<void(lua_State*)>> main_state;
static lua_State* state_;
static lua_State* main_script_thread;
static fs::path bin_path;
static std::span<std::string_view> args;
using Unique_event = std::unique_ptr<library::Event>; 
static void register_event(lua_State* L, Unique_event& ev, const char* fieldname) {
    ev = std::make_unique<library::Event>(L);
    halia::push(L, *ev);
    lua_setfield(L, -2, fieldname);
}

static fs::path res_path() {
    return bin_path / "resources/luau_library";
}
static void register_builtin_library(lua_State* L, Builtin_library& module) {
    module.load(L);
    lua_setfield(L, -2, module.name);
}
[[nodiscard]] Error_message_or<lua_State*> load_script(lua_State* L, const fs::path& script_path) noexcept {
    std::optional<std::string> source = read_file(script_path);
    using namespace std::string_literals;
    if (not source) return std::format("Couldn't read source '{}'.", script_path.string()); 
    auto identifier = script_path.filename().string();
    identifier = "=" + identifier;
    std::string bytecode = Luau::compile(*source, compile_options());
    lua_State* script_thread = lua_newthread(L);
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return std::format("failed to load source: {}", bytecode);
}
static Error_message_or<lua_State*> init_luau_state(const fs::path& main_entry_point) noexcept {
    luaL_openlibs(state_);
    lua_callbacks(state_)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(Namecall_atom::_last);
        try {
            auto e = constext::enum_element<Namecall_atom, count>(name);
            return static_cast<int16_t>(e.value);
        } catch (std::out_of_range& e) {
            luaL_error(state_, e.what());
            return 0i16;
        }
    };
    lua_register_globals(state_);
    lua_newtable(state_);
    lua_setglobal(state_, builtin_name);
    lua_getglobal(state_, builtin_name);
    register_builtin_library(state_, library::filesystem);
    register_builtin_library(state_, library::math);
    register_builtin_library(state_, library::process);
    const luaL_Reg free_functions[] = {
        {"co_wait", library::co_wait},
        {"co_spawn", library::co_spawn},
        {"Event", library::event_ctor},
        {nullptr, nullptr}
    };
    luaL_register(state_, nullptr, free_functions);
    lua_pop(state_, 1);
    luaL_sandbox(state_);
    Error_message_or<lua_State*> outcome = load_script(state_, main_entry_point);
    if (std::string* error = std::get_if<std::string>(&outcome)) {
        return *error;
    }
    //if (not main_thread) return nullptr;
    print("init_state is ok");
    return std::get<lua_State*>(outcome);
}
namespace halia {
namespace internal {
int unique_tag_incr{0};
std::unordered_map<std::string, int> type_registry{};
}
namespace core{
std::optional<std::string> init(const Launch_options& opts) noexcept {
    args = std::move(opts.args);
    main_state = std::unique_ptr<lua_State, std::function<void(lua_State*)>>(luaL_newstate(), [](lua_State* L) -> void {
        printerr("STATE CLOSING");
        lua_close(L);
    });
    state_ = main_state.get();
    bin_path = std::move(opts.bin_path);
    auto result = init_luau_state(opts.main_entry_point);
    if (std::string* err = std::get_if<std::string>(&result)) {
        return *err;
    }
    main_script_thread = std::get<lua_State*>(result);
    return std::nullopt;
}
int bootstrap(const Launch_options& opts) {
    constexpr int loading_error_exit_code = -1;
    constexpr int runtime_error_exit_code = -2;
    if (auto error = init(opts)) {
        printerr(*error);
        return loading_error_exit_code;
    }
    int main_status = lua_resume(main_script_thread, state_, 0);
    while (main_status == LUA_YIELD or not co_tasks::all_done()) {
        if (auto error_message = co_tasks::schedule(state_)) {
            printerr(std::format("runtime error: {}", *error_message));
            return runtime_error_exit_code;
        }
        main_status = lua_status(main_script_thread);
    }
    if (main_status != LUA_OK) {
        const char* error_message = lua_tostring(main_script_thread, -1);
        error_message = error_message ? error_message : "unknown error";

        printerr(std::format("runtime error: {}.", error_message));
        lua_pop(main_script_thread, 1);
        return runtime_error_exit_code;
    }
    return 0;
}
void add_library(const Library_entry &entry) {
    Builtin_library lib(entry.name, entry.loader);
    lua_getglobal(main_state.get(), builtin_name);
    register_builtin_library(main_state.get(), lib);
    lua_pop(main_state.get(), 1);
}
State state() noexcept {
    return state_;
}
Co_thread main_thread() noexcept {
    return main_script_thread;
}
std::span<std::string_view> args_span() noexcept {
    return args;
}
}
}
