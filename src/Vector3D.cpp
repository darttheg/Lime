#include "Vector3D.h"
#include <sol/sol.hpp>
#include "IrrManagers.h"

// Constructors
Vector3D::Vector3D() : x(0), y(0), z(0) {}
Vector3D::Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
Vector3D::Vector3D(float x) : x(x), y(x), z(x) {}

// Operator overloads
Vector3D Vector3D::operator+(const Vector3D& other) const {
    return Vector3D(getX() + other.getX(), getY() + other.getY(), getZ() + other.getZ());
}

Vector3D Vector3D::operator-(const Vector3D& other) const {
    return Vector3D(getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ());
}

Vector3D Vector3D::operator*(float scalar) const {
    return Vector3D(getX() * scalar, getY() * scalar, getZ() * scalar);
}

Vector3D Vector3D::operator/(float scalar) const {
    return Vector3D(getX() / scalar, getY() / scalar, getZ() / scalar);
}

// Length
float Vector3D::length() const {
    return std::sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
}

// Normalize
Vector3D Vector3D::normalize() const {
    return normalizeRange(0, 1);
}

// Normalize to range
Vector3D Vector3D::normalizeRange(float min, float max) const {
    float len = length();
    if (len > 0) {
        float normX = getX()/ len;
        float normY = getY() / len;
        float normZ = getZ() / len;

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
    return (getX()* other.getX()) + (getY() * other.getY()) + (getZ() * other.getZ());
}

// Distance
float Vector3D::distance(const Vector3D& other) const {
    float dx = getX() - other.getX();
    float dy = getY() - other.getY();
    float dz = getZ() - other.getZ();
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Rotate
Vector3D Vector3D::rotate(const Vector3D& rot) const {
    // X-axis rotation
    float cosX = std::cos(rot.getX() * PI / 180.0);
    float sinX = std::sin(rot.getX() * PI / 180.0);
    float newY = getY() * cosX - getZ() * sinX;
    float newZ = getY() * sinX + getZ() * cosX;

    // Y-axis rotation
    float cosY = std::cos(rot.getY() * PI / 180.0);
    float sinY = std::sin(rot.getY() * PI / 180.0);
    float newX = getX() * cosY + newZ * sinY;
    newZ = -getX() * sinY + newZ * cosY;

    // Z-axis rotation
    float cosZ = std::cos(rot.getZ() * PI / 180.0);
    float sinZ = std::sin(rot.getZ() * PI / 180.0);
    float finalX = newX * cosZ - newY * sinZ;
    float finalY = newX * sinZ + newY * cosZ;

    return Vector3D(finalX, finalY, newZ);
}

// Convert to degrees
Vector3D Vector3D::deg() const {
    return Vector3D(getX() * 180 / PI, getY() * 180 / PI, getZ() * 180 / PI);
}

// Convert to radians
Vector3D Vector3D::rad() const {
    return Vector3D(getX() * PI / 180, getY() * PI / 180, getZ() * PI / 180);
}

// Angle between vectors
Vector3D Vector3D::angle(const Vector3D& other) const {
    float RAD2DEG = 180.0f / static_cast<float>(PI);

    Vector3D direction = other - *this;

    if (direction.length() == 0.0f) {
        return Vector3D(0.0f, 0.0f, 0.0f);
    }

    direction = direction.normalize();

    float yaw = std::atan2(direction.getX(), direction.getZ());
    float pitch = std::atan2(-direction.getY(), std::sqrt(direction.getX() * direction.getX() + direction.getZ() * direction.getZ()));
    float roll = 0.0f;

    return Vector3D(pitch * RAD2DEG, yaw * RAD2DEG, roll);
}

// Equality operators
bool Vector3D::operator==(const Vector3D& other) const {
    return getX() == other.getX() && getY() == other.getY() && getZ() == other.getZ();
}

bool Vector3D::operator!=(const Vector3D& other) const {
    return !operator==(other);
}

// To string
std::string Vector3D::toString() const {
    return "(" + std::to_string(getX()) + ", " + std::to_string(getY()) + ", " + std::to_string(getZ()) + ")";
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

        "x", sol::property(&Vector3D::getX, &Vector3D::setX),
        "y", sol::property(&Vector3D::getY, &Vector3D::setY),
        "z", sol::property(&Vector3D::getZ, &Vector3D::setZ),
        sol::meta_function::to_string, &Vector3D::toString);

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