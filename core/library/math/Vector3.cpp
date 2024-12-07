#include "module.hpp"
#include "common/metamethod.hpp"
#include <lualib.h>
#include "common/NamecallAtom.hpp"
#include <lualib.h>
using namespace std::string_literals;
static constexpr auto type = "Vector3";

static int add(lua_State* L) {
    create_raw<Vector3>(L) = check<Vector3>(L, 1) + check<Vector3>(L, 2);
    return 1;
}
static int index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    const auto& self = check<Vector3>(L, 1);
    switch (index) {
        case 'x': lua_pushnumber(L, self.at(0)); return 1;
        case 'y': lua_pushnumber(L, self.at(1)); return 1;
        case 'z': lua_pushnumber(L, self.at(2)); return 1;
        default:
            luaL_error(L, "invalid member");
            return 0;
    }
}
static int newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    auto& self = check<Vector3>(L, 1);
    switch (*luaL_checkstring(L, 2)) {
        case 'x': luaL_error(L, "Property 'X' is readonly."); return 0;
        case 'y': luaL_error(L, "Property 'Y' is readonly"); return 0;
        case 'z': luaL_error(L, "Property 'Z' is readonly."); return 0;
        default:
            luaL_error(L, "invalid member");
            return 0;
    }
    return 0;
}
static int mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    create_raw<Vector3>(L) = check<Vector3>(L, 1) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    const auto& self = check<Vector3>(L, 1);
    double x_v = self.at(0);
    double y_v = self.at(1);
    double z_v = self.at(2);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string z = std::to_string(z_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    if (std::floor(z_v) == z_v) z.erase(z.find('.'));
    std::string str = type + ": {"s + x + ", " + y + ", " + z + "}";
    lua_pushstring(L, str.c_str());
    return 1;
}
static int sub(lua_State* L) {
    return 0;
}
static int div(lua_State *L) {
    return 0;
}
static int unm(lua_State* L) {
    create_raw<Vector3>(L) = -check<Vector3>(L, 1);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<Vector3>(L, 1);
    using A = NamecallAtom;
    switch(static_cast<A>(atom)) {
        case A::dot: {
            const double dot = blaze::dot(check<Vector3>(L, 1), check<Vector3>(L, 2));
            lua_pushnumber(L, dot);
            return 1;
        }
        case A::normalized: {
            create_raw<Vector3>(L) = blaze::normalize(check<Vector3>(L, 1));
            return 1;
        }
        case A::abs: {
            create_raw<Vector3>(L) = blaze::abs(check<Vector3>(L, 1));
            return 0;
        }
        case A::norm: {
            auto& r = check<Vector3>(L, 1);
            lua_pushnumber(L, blaze::length(r));
            return 1;
        };
        default: luaL_error(L, "invalid method name");
    }
    return 0;
}
void module::math::init_vector3_meta(lua_State*L) {
    if (luaL_newmetatable(L, metatable_name<Vector3>())) {
        const luaL_Reg meta[] = {
            {metamethod::index, index},
            {metamethod::add, add},
            {metamethod::mul, mul},
            {metamethod::unm, unm},
            {metamethod::div, div},
            {metamethod::sub, sub},
            {metamethod::namecall, namecall},
            {metamethod::newindex, newindex},
            {metamethod::tostring, tostring},
            {nullptr, nullptr}
    };
    lua_pushstring(L, type);
    lua_setfield(L, -2, metamethod::type);
    luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
}
int module::math::vector3_ctor(lua_State *L) {
    const double x = luaL_optnumber(L, 1, 0);
    const double y = luaL_optnumber(L, 2, 0);
    const double z = luaL_optnumber(L, 3, 0);
    create_raw<Vector3>(L) = {x, y, z};
    return 1;
}
