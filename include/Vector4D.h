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
    float getX() const;
    float getY() const;
    float getZ() const;
    float getW() const;
    void setX(float v);
    void setY(float v);
    void setZ(float v);
    void setW(float v);
};

void bindVector4D();