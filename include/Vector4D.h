#pragma once
#include <string>
#include <cmath>
#include <functional>

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

    using Getter = std::function<Vector4D()>;
    using Setter = std::function<void(const Vector4D&)>;

    Getter get;
    Setter set;

    operator Vector4D() const { return get(); }

    float getX() const { return get().x; }
    float getY() const { return get().y; }
    float getZ() const { return get().z; }
    float getW() const { return get().w; }

    void setX(float v) { auto t = get(); t.x = v; set(t); }
    void setY(float v) { auto t = get(); t.y = v; set(t); }
    void setZ(float v) { auto t = get(); t.z = v; set(t); }
    void setW(float v) { auto t = get(); t.w = v; set(t); }
};

void bindVector4D();