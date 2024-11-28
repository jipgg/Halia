#include "init.h"
#include "userdata_helpers.hpp"
#include "metamethod.h"
constexpr const char* type = "Process_callback";
constexpr std::string_view stdout_key = "stdout";
constexpr std::string_view stderr_key = "stderr";
constexpr std::string_view exit_code_key = "exit_code";

static int index(lua_State* L) {
    const ProcessCallback& self = check<ProcessCallback>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case stdout_key[0]:
            if (key == stdout_key) {
                if (self.std_out) lua_pushstring(L, self.std_out->c_str());
                else lua_pushnil(L);
                return 1;
            } else if (key == stderr_key) {
                if (self.std_err) lua_pushstring(L, self.std_err->c_str());
                else lua_pushnil(L);
                return 1;
            }
        case exit_code_key[0]:
            lua_pushnumber(L, self.exit_code);
            return 1;
    }
    luaL_error(L, "invalid index");
    return 0;
}
static int newindex(lua_State* L) {
    luaL_error(L, "object is readonly");
    return 0;
}

namespace exported {
void init_process_callback_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<ProcessCallback>())) {
        const luaL_Reg meta[] = {
            {metamethod::index, index},
            {metamethod::newindex, newindex},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
}
