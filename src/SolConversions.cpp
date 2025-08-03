#include "SolConversions.h"

Vector2D TblFrom2D(const sol::table& t) {
    return Vector2D(t["x"], t["y"]);
}

Vector3D TblFrom3D(const sol::table& t) {
    return Vector3D(t["x"], t["y"], t["z"]);
}

Vector4D TblFrom4D(const sol::table& t) {
    return Vector4D(t["x"], t["y"], t["z"], t["w"]);
}