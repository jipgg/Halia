#include "init.h"
#include <lualib.h>
#include "library.h"
static const luaL_Reg functions[] = {
    {"Vector2", exported::vector2_ctor},
    {"Vector3", exported::vector3_ctor},
    {nullptr, nullptr}
};
namespace library {
Builtin_library math{"math", [](lua_State* L) {
    exported::init_matrix3_meta(L);
    exported::init_vector2_meta(L);
    exported::init_vector3_meta(L);
    lua_newtable(L);
    luaL_register(L, nullptr, functions);
    exported::matrix3_ctor_table(L);
    lua_setfield(L, -2, "Matrix3");
    return 1;
}};
}
