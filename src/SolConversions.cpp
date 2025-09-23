#include "SolConversions.h"

Vector2D TblFrom2D(const sol::table& t) {
    float x = t.get_or("x", 0.0f);
    float y = t.get_or("y", 0.0f);
    return Vector2D(x, y);
}

Vector3D TblFrom3D(const sol::table& t) {
    float x = t.get_or("x", 0.0f);
    float y = t.get_or("y", 0.0f);
    float z = t.get_or("z", 0.0f);
    return Vector3D(x, y, z);
}

Vector4D TblFrom4D(const sol::table& t) {
    float x = t.get_or("x", 0.0f);
    float y = t.get_or("y", 0.0f);
    float z = t.get_or("z", 0.0f);
    float w = t.get_or("w", 0.0f);
    return Vector4D(x, y, z, w);
}