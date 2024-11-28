#include "init.h"
#include "common/userdata_helpers.h"
#include "common/metamethod.h"
#include <lualib.h>
#include "common/NamecallAtom.h"
#include <sstream>
static constexpr auto type = "waw.meth.Matrix3";
using namespace std::string_literals;

static int err_invalid_vector_size(lua_State* L, int size, int expected) {
    luaL_error(L, "invalid vector size '%d', expected %d", size, expected);
    return 0;
}

static int ctor(lua_State* L) {
    std::array<std::array<double, 3>, 3> arr;
    for (int i{0}; i < 9; ++i) {
        int row = i / 3;
        int col = i % 3;
        double e = luaL_optnumber(L, i + 1, 0);
        arr[row][col] = e;
    }
    create_raw<Matrix3>(L) = Matrix3{arr};
    return 1;
}
static int ctor_call(lua_State* L) {
    auto element = [](lua_State* L, int objidx, int tblidx) {
        if (not lua_istable(L, objidx)) return 0.0;
        lua_rawgeti(L, objidx, tblidx);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            return 0.0;
        }
        const double e = luaL_checknumber(L, -1);
        lua_pop(L, 1);
        return e;
    };
    if (lua_istable(L, 2)) {
        const double e11 = element(L, 2, 1);
        const double e12 = element(L, 2, 2);
        const double e13 = element(L, 2, 3);
        const double e21 = element(L, 3, 1);
        const double e22 = element(L, 3, 2);
        const double e23 = element(L, 3, 3);
        const double e31 = element(L, 4, 1);
        const double e32 = element(L, 4, 2);
        const double e33 = element(L, 4, 3);
        create_raw<Matrix3>(L) = Matrix3{
            {e11, e12, e13},
            {e21, e22, e23},
            {e31, e32, e33},
        };
        return 1;
    }
    std::array<std::array<double, 3>, 3> arr;
    for (int i{0}; i < 9; ++i) {
        int row = i / 3;
        int col = i % 3;
        double e = luaL_optnumber(L, i + 2, 0);
        arr[row][col] = e;
    }
    create_raw<Matrix3>(L) = Matrix3{arr};
    return 1;
}
static int ctor_from_scale(lua_State* L) {
    Vector2 s{};
    if (is_type<Vector2>(L, 1)) {
        auto& v = check<Vector2>(L, 1);
        s = v;
    } else if (lua_isnumber(L, 1)) {
        double num = luaL_checknumber(L, 1);
        s = {num, num};
    } else {
        luaL_error(L, "invalid argument 1");
        return 0;
    }
    create_raw<Matrix3>(L) = Matrix3{
        {s[0], 0, 0},
        {0, s[1], 0},
        {0, 0, 1},
    };
    return 1;
}
static int ctor_from_rotation(lua_State* L) {
    double rad = luaL_checknumber(L, 1);
    create_raw<Matrix3>(L) = Matrix3{
        {cos(rad), -sin(rad), 0},
        {sin(rad), cos(rad), 0},
        {0, 0, 1}
    };
    return 1;
}
static int ctor_from_position(lua_State* L) {
    auto& t = check<Vector2>(L, 1);
    create_raw<Matrix3>(L) = Matrix3{
        {1, 0, t[0]},
        {0, 1, t[1]},
        {0, 0, 1},
    };
    return 1;
}
static int call(lua_State* L) {
    auto& r = check<Matrix3>(L, 1);
    const int i = luaL_checkinteger(L, 2);
    const int j = luaL_checkinteger(L, 3);
    lua_pushnumber(L, r.at(i, j));
    return 1;
}
static int mul(lua_State* L) {
    auto& self = check<Matrix3>(L, 1);
    if (is_type<Matrix3>(L, 2)) {
        auto& rhs = check<Matrix3>(L, 2);
        create_raw<Matrix3>(L) = self * rhs;
        return 1;
    } else if (is_type<Vector3>(L, 2)) {
        create_raw<Vector3>(L) = self * check<Vector3>(L, 2);
        return 1;
    } else if (is_type<Dynamic_vector>(L, 2)) {
        auto& v = check<Dynamic_vector>(L, 2);
        if (v.size() != 3) return err_invalid_vector_size(L, v.size(), 3);
        create<Dynamic_vector>(L, self * v);
        return 1;
    }
    return 0;
}
static int add(lua_State* L) {
    auto& lhs = check<Matrix3>(L, 1);
    auto& rhs = check<Matrix3>(L, 2);
    create_raw<Matrix3>(L) = lhs + rhs;
    return 1;
}
static int sub(lua_State* L) {
    auto& lhs = check<Matrix3>(L, 1);
    auto& rhs = check<Matrix3>(L, 2);
    create_raw<Matrix3>(L) = lhs - rhs;
    return 1;
}
static int tostring(lua_State* L) {
    auto& r = check<Matrix3>(L, 1);
    std::stringstream ss{};
    ss << type << ": {\n    {";
    ss << r.at(0, 0) << ", " << r.at(0, 1) << ", " << r.at(0, 2) << "},\n    {";
    ss << r.at(1, 0) << ", " << r.at(1, 1) << ", " << r.at(1, 2) << "},\n    {";
    ss << r.at(2, 0) << ", " << r.at(2, 1) << ", " << r.at(2, 2) << "}\n}";
    lua_pushlstring(L, ss.str().data(), ss.str().size());
    return 1;
}
static int namecall(lua_State* L) {
    auto& r = check<Matrix3>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    using A = NamecallAtom;
    switch (static_cast<A>(atom)) {
        case A::transpose:
            create_raw<Matrix3>(L) = r.transpose();
        return 1;
        case A::inverse: {
            Matrix3 inv = r;
            blaze::invert3x3<blaze::InversionFlag::asGeneral>(inv);
            create_raw<Matrix3>(L) = inv;
        } return 1;
        default:
        break;
    }
    return 0;
}

namespace exported {
void init_matrix3_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Matrix3>())) {
        const luaL_Reg meta[] = {
            {metamethod::call, call},
            {metamethod::namecall, namecall},
            {metamethod::add, add},
            {metamethod::sub, sub},
            {metamethod::mul, mul},
            {metamethod::tostring, tostring},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
int matrix3_ctor_table(lua_State* L) {
    using namespace std::string_literals;
    const std::string ctor_tname = (type + "_ctor"s);
    if (luaL_newmetatable(L, ctor_tname.c_str())) {
        lua_pushcfunction(L, ctor_call, (ctor_tname + "_call").c_str());
        lua_setfield(L, -2, metamethod::call);
    }
    lua_pop(L, 1);
    const luaL_Reg lib[] = {
        {"from_scale", ctor_from_scale},
        {"from_translation", ctor_from_position},
        {"from_rotation", ctor_from_rotation},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    luaL_getmetatable(L, ctor_tname.c_str());
    lua_setmetatable(L, -2);
    return 1;
}
}
