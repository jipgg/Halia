#include "core.hpp"
#include "type_utils.hpp"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include <Luau/Compiler.h>
#include "common/NamecallAtom.hpp"
#include "lua_base.hpp"
#include "co_tasks.hpp"
#include "common/constext.hpp"
#include "common.hpp"
#include "library.hpp"
#include <stdexcept>
#include "library.hpp"
#include "ErrorInfo.hpp"
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
using halia::ErrorInfo;
static void register_event(lua_State* L, Unique_event& ev, const char* fieldname) {
    ev = std::make_unique<library::Event>(L);
    halia::push(L, *ev);
    lua_setfield(L, -2, fieldname);
}

static fs::path res_path() {
    return bin_path / "resources/luau_library";
}
static void register_builtin_library(lua_State* L, CoreLibrary& module) {
    module.load(L);
    lua_setfield(L, -2, module.name);
}
[[nodiscard]] std::variant<lua_State*, ErrorInfo> load_script(lua_State* L, const fs::path& script_path) noexcept {
    std::optional<std::string> source = read_file(script_path);
    using namespace std::string_literals;
    if (not source) return ErrorInfo(std::format("Couldn't read source '{}'.", script_path.string())); 
    auto identifier = script_path.filename().string();
    identifier = "=" + identifier;
    std::string bytecode = Luau::compile(*source, compile_options());
    lua_State* script_thread = lua_newthread(L);
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return ErrorInfo(std::format("failed to load source: {}", bytecode));
}
std::optional<ErrorInfo> spawn_script(lua_State* L, const fs::path& script_path) noexcept {
    auto ret = load_script(L, script_path);
    if (ErrorInfo* err = std::get_if<ErrorInfo>(&ret)) {
        return err->propagate();
    }
    lua_State* co = std::get<lua_State*>(ret);
    lua_resume(co, L, 0);
    return std::nullopt;
} 
static std::optional<ErrorInfo> init_luau_state() noexcept {
    luaL_openlibs(state_);
    lua_callbacks(state_)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(NamecallAtom::_last);
        try {
            auto e = constext::enum_element<NamecallAtom, count>(name);
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
    register_builtin_library(state_, library::io);
    const luaL_Reg free_functions[] = {
        {"co_wait", library::co_wait},
        {"co_spawn", library::co_spawn},
        {"Event", library::event_ctor},
        {nullptr, nullptr}
    };
    luaL_register(state_, nullptr, free_functions);
    lua_pop(state_, 1);
    luaL_sandbox(state_);
    return std::nullopt;
}
namespace halia::internal {
int unique_tag_incr{0};
std::unordered_map<std::string, int> type_registry{};
}
namespace halia::core{
std::optional<ErrorInfo> init(const LaunchOptions& opts) noexcept {
    args = std::move(opts.args);
    main_state = std::unique_ptr<lua_State, std::function<void(lua_State*)>>(luaL_newstate(), [](lua_State* L) -> void {
        lua_close(L);
    });
    state_ = main_state.get();
    bin_path = std::move(opts.bin_path);
    auto result = init_luau_state();
    if (result) return result->propagate();
    return std::nullopt;
}
std::optional<ErrorInfo> spawn_scripts(std::span<const std::filesystem::path> scripts) {
    constexpr int loading_error_exit_code = -1;
    constexpr int runtime_error_exit_code = -2;
    for (const auto& script : scripts) {
        auto err = spawn_script(state_, script);
        if (err) return err->propagate(); 
    }
    while(not co_tasks::all_done()) {
        if (auto error = co_tasks::schedule(state_)) {
            return error->propagate();
        }
    }
    return std::nullopt;
}
int bootstrap(const LaunchOptions& opts) {
    constexpr int loading_error_exit_code = -1;
    constexpr int runtime_error_exit_code = -2;
    if (auto error = init(opts)) {
        printerr(*error);
        return loading_error_exit_code;
    }
    while (not co_tasks::all_done()) {
        if (auto error = co_tasks::schedule(state_)) {
            printerr(std::format("runtime error: {}", error->formatted()));
            return runtime_error_exit_code;
        }
    }
    return 0;
}
void add_library(const LibraryEntry &entry) {
    CoreLibrary lib(entry.name, entry.loader);
    lua_getglobal(main_state.get(), builtin_name);
    register_builtin_library(main_state.get(), lib);
    lua_pop(main_state.get(), 1);
}
State state() noexcept {
    return state_;
}
CoThread main_thread() noexcept {
    return main_script_thread;
}
std::span<std::string_view> args_span() noexcept {
    return args;
}
}
