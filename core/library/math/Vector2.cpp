#include "module.hpp"
#include "common/metamethod.hpp"
#include <lualib.h>
#include "common/NamecallAtom.hpp"
#include <lualib.h>
using namespace std::string_literals;
static constexpr auto type = "Vector2";

static int add(lua_State* L) {
    const auto& self = check<Vector2>(L, 1);
    const auto& other = check<Vector2>(L, 2);
    create_raw<Vector2>(L) = self + other;
    return 1;
}
static int index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    auto& r = check<Vector2>(L, 1);
    switch (index) {
        case 'x': lua_pushnumber(L, r[0]); return 1;
        case 'y': lua_pushnumber(L, r[1]); return 1;
    }
    return 0;
}
static int newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    auto& r = check<Vector2>(L, 1);
    switch (*luaL_checkstring(L, 2)) {
        case 'x': luaL_error(L, "property 'X' is readonly."); return 0;
        case 'y': luaL_error(L, "Property 'Y' is readonly."); return 0;
        default:
            luaL_error(L, "invalid method");
            return 0;
    }
    return 0;
}
static int mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create_raw<Vector2>(L) = check<Vector2>(L, 1) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    Vector2& r = check<Vector2>(L, 1);
    double x_v = r[0];
    double y_v = r[1];
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    std::string str = type + ": {"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int sub(lua_State* L) {
    return 0;
}
static int div(lua_State *L) {
    return 0;
}
static int unm(lua_State* L) {
    create_raw<Vector2>(L) = -check<Vector2>(L, 1);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<Vector2>(L, 1);
    using A = NamecallAtom;
    switch(static_cast<A>(atom)) {
        case A::dot:
            lua_pushnumber(L, blaze::dot(self, check<Vector2>(L, 2)));
            return 1;
        case A::normalized:
            create_raw<Vector2>(L) = self / blaze::length(self);
            return 1;
        case A::abs:
            create_raw<Vector2>(L) = blaze::abs(self);
            return 1;
        case A::norm:
            lua_pushnumber(L, blaze::length(self));
            return 1;
        default:
        return 0;
    }
}
void module::math::init_vector2_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Vector2>())) {
        const luaL_Reg meta [] = {
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
int module::math::vector2_ctor(lua_State *L) {
    if (lua_isnone(L, 1)) {//default constructor
        create_raw<Vector2>(L) = {};
        return 1;
    } else if (is_type<Vector2>(L, 1)) {//copy constructor
        auto& v = check<Vector2>(L, 1);
        create_raw<Vector2>(L) = v;
        return 1;
    } else if (lua_isnumber(L, 1)) {
        create_raw<Vector2>(L) = {luaL_checknumber(L, 1), luaL_optnumber(L, 2, 0)};
        return 1;
    } else if (lua_istable(L, 1)) {//from table
        double x{}, y{};
        if (lua_rawgeti(L, 1, 1) == LUA_TNUMBER) {
            x = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        if (lua_rawgeti(L, 1, 2) == LUA_TNUMBER) {
            y = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        if (lua_rawgetfield(L, 1, "x") == LUA_TNUMBER) {
            x = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        if (lua_rawgetfield(L, 1, "y") == LUA_TNUMBER) {
            y = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        create_raw<Vector2>(L) = {x, y};
        return 1;
    }
    luaL_error(L, "invalid initializer arguments");
    return 0;
}
