#include "init.hpp"
#include "common/metamethod.hpp"
constexpr const char* type = "process_Execution_feedback";
constexpr std::string_view output_key = "output";
constexpr std::string_view error_key = "error";
constexpr std::string_view exit_code_key = "exit_code";
constexpr std::string_view failed_before_execution_key = "failed_before_execution";
using namespace std::string_view_literals;

static int index(lua_State* L) {
    const Execution_feedback& self = halia::check<Execution_feedback>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case output_key[0]:
            if (key == output_key and self.output) lua_pushstring(L, self.output->c_str());
            else lua_pushnil(L);
            return 1;
        case exit_code_key[0]:
            if (key == exit_code_key){
                lua_pushnumber(L, self.exit_code);
                return 1;
            } else if (key == error_key) {
                if (self.error) lua_pushstring(L, self.error->c_str());
                else lua_pushnil(L);
                return 1;
            }
        case failed_before_execution_key[0]:
            if (key == failed_before_execution_key) lua_pushboolean(L, self.failed_before_execution);
            return 1;
    }
    luaL_error(L, "invalid index");
    return 0;
}
static int tostring(lua_State* L) {
    const Execution_feedback& self = halia::check<Execution_feedback>(L, 1);
    std::string v = std::string(type) + "{\n  .output = " + (self.output ? '\"' + *self.output + '\"' : "nil");
    v += ",\n  .error = " + (self.error ? '\"' + *self.error + '\"' : "nil");
    v += ",\n  .exit_code = " + std::to_string(self.exit_code);
    v += ",\n  .failed_before_execution = " + std::string(self.failed_before_execution ? "true" : "false");
    v += "\n}";
    lua_pushstring(L, v.c_str());
    return 1;
}
static int newindex(lua_State* L) {
    luaL_error(L, "object is readonly");
    return 0;
}

namespace exported {
void init_execution_feedback_meta(lua_State* L) {
    if (luaL_newmetatable(L, halia::metatable_name<Execution_feedback>())) {
        const luaL_Reg meta[] = {
            {metamethod::index, index},
            {metamethod::newindex, newindex},
            {metamethod::tostring, tostring},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
}
