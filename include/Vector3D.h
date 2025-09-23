#pragma once
#include <string>
#include <cmath>
#include <functional>

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
    
    using Getter = std::function<Vector3D()>;
    using Setter = std::function<void(const Vector3D&)>;

    Getter get;
    Setter set;

    operator Vector3D() const { return get(); }

    float getX() const { return get().x; }
    float getY() const { return get().y; }
    float getZ() const { return get().z; }

    void setX(float v) { auto t = get(); t.x = v; set(t); }
    void setY(float v) { auto t = get(); t.y = v; set(t); }
    void setZ(float v) { auto t = get(); t.z = v; set(t); }
};

void bindVector3D();