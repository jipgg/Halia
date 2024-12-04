#include "here.hpp"
#include "type_utils.hpp"
#include "common/Namecall_atom.hpp"
#include "common/metamethod.hpp"
constexpr const char* type = "Args_span";
constexpr std::string_view size_key = "size";

static int namecall(lua_State* L) {
    auto& self = check<Args_span>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    using Na = Namecall_atom;
    switch (static_cast<Na>(atom)) {
        case Na::at: {
            const int idx = luaL_checkinteger(L, 2);
            if (idx >= self.size()) {
                luaL_argerrorL(L, 2, "index is out of range."); 
                return 0;
            }
            lua_pushlstring(L, self[idx].data(), self[idx].size());
            return 1;
        }
        default:
            luaL_errorL(L, "invalid namecall");
    }
    return 0;
}
static int index(lua_State* L) {
    auto& self = check<Args_span>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case size_key[0]:
            if (key == size_key) {
                lua_pushinteger(L, self.size());
                return 1;
            }
    }
    luaL_errorL(L, "invalid index");
    return 0;
}
static int newindex(lua_State* L) {
    luaL_errorL(L, "is readonly");
    return 0;
}
static const luaL_Reg meta[] = {
    {metamethod::namecall, namecall},
    {metamethod::index, index},
    {metamethod::newindex, newindex},
    {nullptr, nullptr}
};

namespace exported {
void init_args_span_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Args_span>())) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
}
