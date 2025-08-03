#include "Vector3D.h"
#include <sol/sol.hpp>
#include "IrrManagers.h"

// Constructors
Vector3D::Vector3D() : x(0), y(0), z(0) {}
Vector3D::Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
Vector3D::Vector3D(float x) : x(x), y(x), z(x) {}

// Operator overloads
Vector3D Vector3D::operator+(const Vector3D& other) const {
    return Vector3D(x + other.x, y + other.y, z + other.z);
}

Vector3D Vector3D::operator-(const Vector3D& other) const {
    return Vector3D(x - other.x, y - other.y, z - other.z);
}

Vector3D Vector3D::operator*(float scalar) const {
    return Vector3D(x * scalar, y * scalar, z * scalar);
}

Vector3D Vector3D::operator/(float scalar) const {
    return Vector3D(x / scalar, y / scalar, z / scalar);
}

// Length
float Vector3D::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

// Normalize
Vector3D Vector3D::normalize() const {
    return normalizeRange(0, 1);
}

// Normalize to range
Vector3D Vector3D::normalizeRange(float min, float max) const {
    float len = length();
    if (len > 0) {
        float normX = x / len;
        float normY = y / len;
        float normZ = z / len;

        float scale = max - min;
        normX = normX * scale + min;
        normY = normY * scale + min;
        normZ = normZ * scale + min;

        return Vector3D(normX, normY, normZ);
    }
    return Vector3D(0.0f, 0.0f, 0.0f);
}

// Dot product
float Vector3D::dot(const Vector3D& other) const {
    return (x * other.x) + (y * other.y) + (z * other.z);
}

// Distance
float Vector3D::distance(const Vector3D& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Rotate
Vector3D Vector3D::rotate(const Vector3D& rot) const {
    // X-axis rotation
    float cosX = std::cos(rot.x * 180.0 / PI);
    float sinX = std::sin(rot.x * 180.0 / PI);
    float newY = y * cosX - z * sinX;
    float newZ = y * sinX + z * cosX;

    // Y-axis rotation
    float cosY = std::cos(rot.y * 180.0 / PI);
    float sinY = std::sin(rot.y * 180.0 / PI);
    float newX = x * cosY + newZ * sinY;
    newZ = -x * sinY + newZ * cosY;

    // Z-axis rotation
    float cosZ = std::cos(rot.z * 180.0 / PI);
    float sinZ = std::sin(rot.z * 180.0 / PI);
    float finalX = newX * cosZ - newY * sinZ;
    float finalY = newX * sinZ + newY * cosZ;

    finalX = finalX * PI / 180.0;
    finalY = finalY * PI / 180.0;
    newZ = newZ * PI / 180.0;

    return Vector3D(finalX, finalY, newZ);
}

// Convert to degrees
Vector3D Vector3D::deg() const {
    return Vector3D(x * 180 / PI, y * 180 / PI, z * 180 / PI);
}

// Convert to radians
Vector3D Vector3D::rad() const {
    return Vector3D(x * PI / 180, y * PI / 180, z * PI / 180);
}

// Angle between vectors
Vector3D Vector3D::angle(const Vector3D& other) const {
    float RAD2DEG = 180.0f / static_cast<float>(PI);

    Vector3D direction = other - *this;

    if (direction.length() == 0.0f) {
        return Vector3D(0.0f, 0.0f, 0.0f);
    }

    direction = direction.normalize();

    float yaw = std::atan2(direction.x, direction.z);
    float pitch = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));
    float roll = 0.0f;

    return Vector3D(pitch * RAD2DEG, yaw * RAD2DEG, roll);
}

// Equality operators
bool Vector3D::operator==(const Vector3D& other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3D::operator!=(const Vector3D& other) const {
    return !operator==(other);
}

// To string
std::string Vector3D::toString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

// Lua binding
void bindVector3D() {
    sol::usertype<Vector3D> bindType = lua->new_usertype<Vector3D>("Vector3D",
        sol::constructors<Vector3D(), Vector3D(float, float, float), Vector3D(float)>(),
        sol::meta_function::addition, &Vector3D::operator+,
        sol::meta_function::subtraction, &Vector3D::operator-,
        sol::meta_function::multiplication, &Vector3D::operator*,
        sol::meta_function::division, &Vector3D::operator/,
        sol::meta_function::equal_to, &Vector3D::operator==,

        "x", &Vector3D::x,
        "y", &Vector3D::y,
        "z", &Vector3D::z);

    bindType["length"] = &Vector3D::length;
    bindType["normalize"] = &Vector3D::normalize;
    bindType["normalizeToRange"] = &Vector3D::normalizeRange;
    bindType["dot"] = &Vector3D::dot;
    bindType["distance"] = &Vector3D::distance;
    bindType["toDegrees"] = &Vector3D::deg;
    bindType["toRadians"] = &Vector3D::rad;
    bindType["rotate"] = &Vector3D::rotate;
    bindType["angle"] = &Vector3D::angle;
    bindType["toStr"] = &Vector3D::toString;
}