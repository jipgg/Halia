#include "module.hpp"
#include <lualib.h>
#include "library.hpp"
static const luaL_Reg functions[] = {
    {"Vector2", module::math::vector2_ctor},
    {"Vector3", module::math::vector3_ctor},
    {nullptr, nullptr}
};
Builtin_library library::math{"math", [](lua_State* L) {
    module::math::init_matrix3_meta(L);
    module::math::init_vector2_meta(L);
    module::math::init_vector3_meta(L);
    lua_newtable(L);
    luaL_register(L, nullptr, functions);
    module::math::matrix3_ctor_table(L);
    lua_setfield(L, -2, "Matrix3");
    return 1;
}};
