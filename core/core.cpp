#include "core.hpp"
#include "type_utils.hpp"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "builtin.hpp"
#include <Luau/Compiler.h>
#include "common/Namecall_atom.hpp"
#include "lua_base.hpp"
#include "common/comptime_enum.hpp"
#include "common/common.hpp"
#include <stdexcept>
#include "Cothread.hpp"
#include "library.hpp"
#include <string_view>
#include <variant>
#include <format>
namespace fs = std::filesystem;
static constexpr auto builtin_name = "std";
static lua_State* main_state;
static fs::path bin_path;
static std::span<std::string_view> args;
using Unique_event = std::unique_ptr<builtin::Event>; 
static void register_event(lua_State* L, Unique_event& ev, const char* fieldname) {
    ev = std::make_unique<builtin::Event>(L);
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
[[nodiscard]] Error_message_or<lua_State*> load_script(lua_State* L, const fs::path& script_path) {
    std::optional<std::string> source = read_file(script_path);
    using namespace std::string_literals;
    if (not source) return std::format("Couldn't read source '{}'.", script_path.string()); 
    auto identifier = script_path.filename().string();
    identifier = "=" + identifier;
    std::string bytecode = Luau::compile(*source, compile_options());
    lua_State* main_thread = lua_newthread(L);
    const int load_status = luau_load(main_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(main_thread);
        return main_thread;
    }
    return std::format("failed to load source: {}", bytecode);
}
static Error_message_or<lua_State*> init_luau_state(const fs::path& main_entry_point) {
    luaL_openlibs(main_state);
    lua_callbacks(main_state)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(Namecall_atom::_last);
        try {
            auto e = comptime_enum::item<Namecall_atom, count>(name);
            return static_cast<int16_t>(e.index);
        } catch (std::out_of_range& e) {
            luaL_error(main_state, e.what());
            return 0i16;
        }
    };
    lua_register_globals(main_state);
    lua_newtable(main_state);
    lua_setglobal(main_state, builtin_name);
    builtin::register_event_type(main_state);
    lua_getglobal(main_state, builtin_name);
    register_builtin_library(main_state, library::filesystem);
    register_builtin_library(main_state, library::math);
    register_builtin_library(main_state, library::process);
    const luaL_Reg free_functions[] = {
        {"co_wait", library::co_wait},
        {"co_spawn", library::co_spawn},
        {nullptr, nullptr}
    };
    luaL_register(main_state, nullptr, free_functions);
    lua_pop(main_state, 1);
    luaL_sandbox(main_state);
    Error_message_or<lua_State*> outcome = load_script(main_state, main_entry_point);
    if (std::string* error = std::get_if<std::string>(&outcome)) {
        return *error;
    }
    //if (not main_thread) return nullptr;
    print("init_state is ok");
    return std::get<lua_State*>(outcome);
}
static Error_message_or<lua_State*> init(halia::core::Launch_options opts) {
    args = std::move(opts.args);
    main_state = luaL_newstate();
    bin_path = std::move(opts.bin_path);
    return init_luau_state(opts.main_entry_point);
}
namespace halia {
namespace intern {
int unique_tag_incr{0};
std::unordered_map<std::string, int> type_registry{};
}
namespace core{
void emplace_cothread(Cothread&& co) {
}
int bootstrap(Launch_options opts) {
    constexpr int loading_error_code = -1;
    constexpr int runtime_error_code = -2;
    Scope_guard thou_shalt_close([] {
        lua_close(main_state);
    });
    Error_message_or<lua_State*> outcome = init(opts);
    if (std::string* error = std::get_if<std::string>(&outcome)) {
        printerr(*error);
        return loading_error_code;
    }
    lua_State* main_thread = std::get<lua_State*>(outcome);
    int main_status = lua_resume(main_thread, main_state, 0);
    while (main_status == LUA_YIELD or not co_tasks::all_tasks_done()) {
        if (Error_message_on_failure error_message = co_tasks::schedule_tasks(main_state)) {
            printerr(std::format("runtime error: {}", *error_message));
            return runtime_error_code;
        }
        main_status = lua_status(main_thread);
    }
    if (main_status != LUA_OK) {
        const char* error_message = lua_tostring(main_thread, -1);
        error_message = error_message ? error_message : "unknown error";

        printerr(std::format("runtime error: {}.", error_message));
        lua_pop(main_thread, 1);
        return runtime_error_code;
    }
    return 0;
}
void add_library(const Library_entry &entry) {
    Builtin_library lib(entry.name, entry.loader);
    lua_getglobal(main_state, builtin_name);
    register_builtin_library(main_state, lib);
    lua_pop(main_state, 1);
}
lua_State* lua_state() noexcept {
    return main_state;
}
std::span<std::string_view> args_span() noexcept {
    return args;
}
}
}
