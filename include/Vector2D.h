#pragma once
#include <string>
#include <cmath>

#define PI 3.14159265358979323846

class Vector2D {
public:
    float x, y;

    Vector2D();
    Vector2D(float x, float y);
    Vector2D(float x);

    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(float scalar) const;
    Vector2D operator/(float scalar) const;

    float length() const;
    Vector2D normalize() const;
    Vector2D normalizeRange(float min = 0, float max = 0) const;
    float dot(const Vector2D& other) const;
    float distance(const Vector2D& other = Vector2D(0, 0)) const;
    Vector2D rotate(float rad = 0) const;
    float angle(const Vector2D& other = Vector2D(0, 0)) const;

    bool operator==(const Vector2D& other) const;
    bool operator!=(const Vector2D& other) const;

    std::string toString() const;
};

void bindVector2D();