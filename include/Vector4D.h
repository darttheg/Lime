#pragma once
#include <string>
#include <cmath>

class Vector4D {
public:
    float x, y, z, w;

    Vector4D();
    Vector4D(float x, float y, float z, float w);
    Vector4D(float x);

    Vector4D operator+(const Vector4D& other) const;
    Vector4D operator-(const Vector4D& other) const;
    Vector4D operator*(float scalar) const;
    Vector4D operator/(float scalar) const;

    bool operator==(const Vector4D& other) const;
    bool operator!=(const Vector4D& other) const;

    std::string toString() const;
};

void bindVector4D();