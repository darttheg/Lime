#include "Vector2D.h"
#include <sol/sol.hpp>
#include "IrrManagers.h"

// Constructors
Vector2D::Vector2D() : x(0), y(0) {}
Vector2D::Vector2D(float x, float y) : x(x), y(y) {}
Vector2D::Vector2D(float x) : x(x), y(x) {}

// Operator overloads
Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(getX() + other.getX(), getY() + other.getY());
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(getX() - other.getX(), getY() - other.getY());
}

Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D(getX() * scalar, getY() * scalar);
}

Vector2D Vector2D::operator/(float scalar) const {
    return Vector2D(getX() / scalar, getY() / scalar);
}

// Length
float Vector2D::length() const {
    return std::sqrt(getX() * getX() + getY() * getY());
}

// Normalize
Vector2D Vector2D::normalize() const {
    return normalizeRange(0, 1);
}

// Normalize to range
Vector2D Vector2D::normalizeRange(float min, float max) const {
    float len = length();
    if (len > 0) {
        float normX = x / len;
        float normY = y / len;

        float scale = max - min;
        normX = normX * scale + min;
        normY = normY * scale + min;

        return Vector2D(normX, normY);
    }
    return Vector2D(0.0f, 0.0f);
}

// Dot product
float Vector2D::dot(const Vector2D& other) const {
    return (getX() * other.getX()) + (getY() * other.getY());
}

// Distance
float Vector2D::distance(const Vector2D& other) const {
    float dx = getX() - other.getX();
    float dy = getY() - other.getY();
    return std::sqrt(dx * dx + dy * dy);
}

// Rotate
Vector2D Vector2D::rotate(float rad) const {
    float cosTheta = std::cos(rad);
    float sinTheta = std::sin(rad);

    return Vector2D(
        getX() * cosTheta - getY() * sinTheta,
        getX() * sinTheta + getY() * cosTheta
    );
}

// Angle between vectors
float Vector2D::angle(const Vector2D& other) const {
    float thisDot = dot(other);
    float lenProduct = length() * other.length();

    if (lenProduct == 0) {
        return 0.0f;
    }

    float cosTheta = thisDot / lenProduct;
    cosTheta = std::fmax(-1.0f, std::fmin(1.0f, cosTheta));
    return std::acos(cosTheta);
}

// Equality operators
bool Vector2D::operator==(const Vector2D& other) const {
    return getX() == other.getX() && getY() == other.getY();
}

bool Vector2D::operator!=(const Vector2D& other) const {
    return !(getX() == other.getX() && getY() == other.getY());
}

// To string
std::string Vector2D::toString() const {
    return "(" + std::to_string(getX()) + ", " + std::to_string(getY()) + ")";
}

// Lua binding
void bindVector2D() {
    sol::usertype<Vector2D> bindType = lua->new_usertype<Vector2D>("Vector2D",
        sol::constructors<Vector2D(), Vector2D(float, float), Vector2D(float)>(),
        sol::meta_function::addition, &Vector2D::operator+,
        sol::meta_function::subtraction, &Vector2D::operator-,
        sol::meta_function::multiplication, &Vector2D::operator*,
        sol::meta_function::division, &Vector2D::operator/,
        sol::meta_function::equal_to, &Vector2D::operator==,
        sol::meta_function::type, [](const Vector2D&) { return "Vector2D"; },

        "x", sol::property(&Vector2D::getX, &Vector2D::setX),
        "y", sol::property(&Vector2D::getY, &Vector2D::setY),
        sol::meta_function::to_string, &Vector2D::toString);

    bindType["length"] = &Vector2D::length;
    bindType["normalize"] = &Vector2D::normalize;
    bindType["normalizeToRange"] = &Vector2D::normalizeRange;
    bindType["dot"] = &Vector2D::dot;
    bindType["distance"] = &Vector2D::distance;
    bindType["angle"] = &Vector2D::angle;
    bindType["rotate"] = &Vector2D::rotate;
    bindType["toStr"] = &Vector2D::toString;
}