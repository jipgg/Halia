#pragma once
#include <type_utils.hpp>
#include <blaze/Blaze.h>
using namespace halia;
struct lua_State;

using Vector2 = blaze::StaticVector<double, 2, blaze::defaultTransposeFlag, blaze::aligned, blaze::unpadded>;
using Vector3 = blaze::StaticVector<double, 3, blaze::defaultTransposeFlag, blaze::aligned, blaze::unpadded>;
using Matrix3 = blaze::StaticMatrix<double, 3, 3>;
using DynamicVector = blaze::DynamicVector<double>;

namespace module::math {
void init_vector3_meta(lua_State* L);
int vector3_ctor(lua_State* L);
void init_vector2_meta(lua_State* L);
int vector2_ctor(lua_State* L);
void init_matrix3_meta(lua_State* L);
int matrix3_ctor_table(lua_State* L);
}
