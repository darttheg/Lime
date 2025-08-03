#pragma once
#include <string>
#include <cmath>

#define PI 3.14159265358979323846

class Vector3D {
public:
    float x, y, z;

    Vector3D();
    Vector3D(float x, float y, float z);
    Vector3D(float x);

    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D operator*(float scalar) const;
    Vector3D operator/(float scalar) const;

    float length() const;
    Vector3D normalize() const;
    Vector3D normalizeRange(float min, float max) const;
    float dot(const Vector3D& other = Vector3D(0, 0, 0)) const;
    float distance(const Vector3D& other = Vector3D(0, 0, 0)) const;
    Vector3D rotate(const Vector3D& rot = Vector3D(0, 0, 0)) const;
    Vector3D deg() const;
    Vector3D rad() const;
    Vector3D angle(const Vector3D& other = Vector3D(0, 0, 0)) const;

    bool operator==(const Vector3D& other) const;
    bool operator!=(const Vector3D& other) const;

    std::string toString() const;
};

void bindVector3D();