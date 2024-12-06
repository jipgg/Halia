#include "module.hpp"
#include "common/metamethod.hpp"
constexpr const char* type = "Process_id";

static int tostring(lua_State* L) {
    const ProcessID& id = check<ProcessID>(L, 1);
    lua_pushstring(L, std::to_string(id).c_str());
    return 1;
}
static const luaL_Reg meta[] = {
    {metamethod::tostring, tostring},
    {nullptr, nullptr}
};
void module::process::init_pid_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<ProcessID>())) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
