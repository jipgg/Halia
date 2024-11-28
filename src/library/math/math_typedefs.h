#pragma once
#include <blaze/Blaze.h>

using Vector2 = blaze::StaticVector<double, 2, blaze::defaultTransposeFlag, blaze::aligned, blaze::unpadded>;
using Vector3 = blaze::StaticVector<double, 3, blaze::defaultTransposeFlag, blaze::aligned, blaze::unpadded>;
using Matrix3 = blaze::StaticMatrix<double, 3, 3>;
using Dynamic_vector = blaze::DynamicVector<double>;
