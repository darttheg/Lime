#pragma once
#include <sol/sol.hpp>
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"

Vector2D TblFrom2D(const sol::table&);
Vector3D TblFrom3D(const sol::table&);
Vector4D TblFrom4D(const sol::table&);