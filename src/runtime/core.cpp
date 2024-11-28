#include "runtime.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "builtin.h"
#include <Luau/Compiler.h>
#include "lua_atom.h"
#include "lua_base.h"
#include "comptime_enum.h"
#include "common.h"
#include <stdexcept>
#include "library.h"
#include <string_view>
namespace fs = std::filesystem;
static constexpr auto builtin_name = "waw";
static lua_State* main_state;
static fs::path bin_path;

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
    } else {
        auto identifier = script.filename().string();
        identifier = "=" + identifier;
        std::string bytecode = Luau::compile(*source, compile_options());
        if (luau_load(L, identifier.c_str(), bytecode.data(), bytecode.size(), 0)) {
            printerr(luaL_checkstring(L, -1));
        } else {
            if (lua_pcall(main_state, 0, 0, 0)) {
                printerr("failed to call", luaL_checkstring(main_state, -1));
            }
        }
    }
    
}
static void init_luau_state(const fs::path& main_entry_point) {
    luaL_openlibs(main_state);
    lua_callbacks(main_state)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(lua_atom::_last);
        try {
            auto e = comptime_enum::item<lua_atom, count>(name);
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
    lua_pop(main_state, 1);
    execute_script(main_state, main_entry_point);
    luaL_sandbox(main_state);
}
static void init(runtime::Launch_options opts) {
    main_state = luaL_newstate();
    bin_path = std::move(opts.bin_path);
    init_luau_state(opts.main_entry_point);
}
namespace runtime {
int bootstrap(runtime::Launch_options opts) {
    init(opts);
    return 0;
}
}
