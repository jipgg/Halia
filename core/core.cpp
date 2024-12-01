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
#include "library.hpp"
#include <string_view>
namespace fs = std::filesystem;
static constexpr auto builtin_name = "std";
static lua_State* main_state;
static fs::path bin_path;
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

static void execute_script(lua_State* L, const fs::path& script) {
    std::optional<std::string> source = read_file(script);
    if (not source) {
        using namespace std::string_literals;
        printerr("failed to read the file '"s + script.string() + "'");
        return;
    }
    auto identifier = script.filename().string();
    identifier = "=" + identifier;
    std::string bytecode = Luau::compile(*source, compile_options());
    if (luau_load(L, identifier.c_str(), bytecode.data(), bytecode.size(), 0)) {
        printerr(luaL_checkstring(L, -1));
        return;
    }
    const bool expected = lua_pcall(L, 0, 0, 1) == LUA_OK;
    if (!expected) printerr("Execution error in ", identifier, lua_tostring(L, -1));
}
static void init_luau_state(const fs::path& main_entry_point) {
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
    lua_pop(main_state, 1);
    execute_script(main_state, main_entry_point);
    luaL_sandbox(main_state);
}
static void init(halia::core::Launch_options opts) {
    main_state = luaL_newstate();
    bin_path = std::move(opts.bin_path);
    init_luau_state(opts.main_entry_point);
}
namespace halia {
namespace intern {
int unique_tag_incr{0};
std::unordered_map<std::string, int> type_registry{};
}
namespace core{
int bootstrap(Launch_options opts) {
    init(opts);
    return 0;
}
void add_library(const Library_entry &entry) {
    Builtin_library lib(entry.name, entry.loader);
    lua_getglobal(main_state, builtin_name);
    register_builtin_library(main_state, lib);
    lua_pop(main_state, 1);
}
lua_State* lua_state() {
    return main_state;
}
}
}
