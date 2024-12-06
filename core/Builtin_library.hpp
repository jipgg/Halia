#pragma once
#include <lualib.h>
constexpr auto module_cache_name = "__builtin_module_cache";
struct CoreLibrary {
    static constexpr int not_initialized = -1;
    inline static int builtin_module_cache_ref{not_initialized};
    const char* const name;
    const lua_CFunction loader;
    bool loaded{false};
    CoreLibrary(const char* name, lua_CFunction loader):
        name(name),
        loader(loader),
        loaded(false) {
    }
    int load(lua_State* L) {
        if (loaded) {
            lua_getref(L, builtin_module_cache_ref);
            lua_getfield(L, -1, name);
            lua_remove(L, -2);
            return 1;
        }
        if (builtin_module_cache_ref == not_initialized) {
            lua_newtable(L);
            builtin_module_cache_ref = lua_ref(L, -1);
            lua_pop(L, 1);
        }
        lua_getref(L, builtin_module_cache_ref);
        lua_getfield(L, -1, name);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            if (loader(L) != 1) luaL_error(L, "Library function did not return exactly one value");
            lua_setfield(L, -2, name);
            lua_getfield(L, -1, name);
            loaded = true;
        }
        lua_remove(L, -2);
        return 1;
    }
    bool push_if_callback(const char* listener, lua_State* L) {
        if (not loaded) return false;
        lua_getglobal(L, module_cache_name);
        lua_getfield(L, -1, name);
        lua_remove(L, -2);
        lua_getfield(L, -1, listener);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 2);
            return false;
        }
        lua_remove(L, -2);
        if (not lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            return false;
        }
        return true;
    }
};
