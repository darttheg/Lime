#include "Vector4D.h"
#include <sol/sol.hpp>
#include "IrrManagers.h"

// Constructors
Vector4D::Vector4D() : x(0), y(0), z(0), w(0) {}
Vector4D::Vector4D(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Vector4D::Vector4D(float x) : x(x), y(x), z(x), w(x) {}

// Operator overloads
Vector4D Vector4D::operator+(const Vector4D& other) const {
    return Vector4D(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4D Vector4D::operator-(const Vector4D& other) const {
    return Vector4D(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4D Vector4D::operator*(float scalar) const {
    return Vector4D(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4D Vector4D::operator/(float scalar) const {
    return Vector4D(x / scalar, y / scalar, z / scalar, w / scalar);
}

// Equality operators
bool Vector4D::operator==(const Vector4D& other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Vector4D::operator!=(const Vector4D& other) const {
    return !operator==(other);
}

// To string
std::string Vector4D::toString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
}

// Lua binding
void bindVector4D() {
    sol::usertype<Vector4D> bindType = lua->new_usertype<Vector4D>("Vector4D",
        sol::constructors<Vector4D(), Vector4D(float, float, float, float), Vector4D(float)>(),
        sol::meta_function::addition, &Vector4D::operator+,
        sol::meta_function::subtraction, &Vector4D::operator-,
        sol::meta_function::multiplication, &Vector4D::operator*,
        sol::meta_function::division, &Vector4D::operator/,
        sol::meta_function::equal_to, &Vector4D::operator==,

        "x", &Vector4D::x,
        "y", &Vector4D::y,
        "z", &Vector4D::z,
        "w", &Vector4D::w);
    bindType["toStr"] = &Vector4D::toString;
}