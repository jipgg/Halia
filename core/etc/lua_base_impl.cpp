#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include <Luau/Common.h>
#include <Luau/Compiler.h>
#include <Luau/CodeGen.h>
#include "Require.hpp"
#include "common.hpp"
#include "lua_base.hpp"
namespace fs = std::filesystem;
struct Global_options {
    int optimizationLevel = 2;
    int debugLevel = 1;
} global_opts;
static bool codegen = true;
Luau::CompileOptions compile_options() {
    Luau::CompileOptions result = {};
    result.optimizationLevel = global_opts.optimizationLevel;
    result.debugLevel = global_opts.debugLevel;
    result.typeInfoLevel = 1;
    //result.userdataTypes = userdata_types;
    return result;
}
static int lua_loadstring(lua_State* L) {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);
    lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
    std::string bytecode = Luau::compile(std::string(s, l), compile_options());
    if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0)
        return 1;
    lua_pushnil(L);
    lua_insert(L, -2); // put before error message
    return 2;          // return nil plus error message
}
static int finishrequire(lua_State* L) {
    if (lua_isstring(L, -1))
        lua_error(L);
    return 1;
}
static int lua_require(lua_State* L) {
    std::string name = luaL_checkstring(L, 1);
    RequireResolver::ResolvedRequire resolved_require = RequireResolver::resolve_require(L, std::move(name));
    if (resolved_require.status == RequireResolver::module_status::cached)
        return finishrequire(L);
    else if (resolved_require.status == RequireResolver::module_status::not_found)
        luaL_errorL(L, "error requiring module");
    lua_State* GL = lua_mainthread(L);
    lua_State* ML = lua_newthread(GL);
    lua_xmove(GL, L, 1);
    luaL_sandboxthread(ML);
    std::string bytecode = Luau::compile(resolved_require.source_code, compile_options());
    if (luau_load(ML, resolved_require.chunkname.c_str(), bytecode.data(), bytecode.size(), 0) == 0) {
        if (codegen) {
            Luau::CodeGen::CompilationOptions nativeOptions;
            Luau::CodeGen::compile(ML, -1, nativeOptions);
        }
        int status = lua_resume(ML, L, 0);
        if (status == 0) {
            if (lua_gettop(ML) == 0)
                lua_pushstring(ML, "module must return a value");
            else if (!lua_istable(ML, -1) && !lua_isfunction(ML, -1))
                lua_pushstring(ML, "module must return a table or function");
        } else if (status == LUA_YIELD) {
            lua_pushstring(ML, "module can not yield");
        } else if (!lua_isstring(ML, -1)) {
            lua_pushstring(ML, "unknown error while running module");
        }
    }
    lua_xmove(ML, L, 1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -4, resolved_require.absolute_path.c_str());
    return finishrequire(L);
}
static int lua_collectgarbage(lua_State* L) {
    const char* option = luaL_optstring(L, 1, "collect");
    if (strcmp(option, "collect") == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        return 0;
    }
    if (strcmp(option, "count") == 0) {
        int c = lua_gc(L, LUA_GCCOUNT, 0);
        lua_pushnumber(L, c);
        return 1;
    }
    luaL_error(L, "collectgarbage must be called with 'count' or 'collect'");
}
void push_luau_module(lua_State* L, const fs::path& path) {
    auto source = read_file(path);
    assert(source);
    std::string name = path.stem().string();
    std::string bytecode = Luau::compile(*source, compile_options());
    if (luau_load(L, name.c_str(), bytecode.data(), bytecode.size(), 0)) {
        printerr("failed to load", name.c_str(),"library:", bytecode);
    }
    lua_pcall(L, 0, 1, 0);
}
void set_luau_module_global(lua_State* L, const fs::path& path) {
    std::string filename = path.stem().string();
    push_luau_module(L, path);

    lua_setglobal(L, filename.c_str());
}

void lua_register_globals(lua_State *L) {
    static const luaL_Reg global_functions[] = {
        {"loadstring", lua_loadstring},
        {"require", lua_require},
        {"collectgarbage", lua_collectgarbage},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}
