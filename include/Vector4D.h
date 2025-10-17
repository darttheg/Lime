#pragma once
#include <string>
#include <cmath>
#include <functional>

class Vector4D {
    float x, y, z, w;
public:

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

    float getX() const { if (get) return get().x; else return x; }
    float getY() const { if (get) return get().y; else return y; }
    float getZ() const { if (get) return get().z; else return z; }
    float getW() const { if (get) return get().w; else return w; }

    void setX(float v) { if (get) { auto t = get(); t.x = v; set(t); } else x = v; }
    void setY(float v) { if (get) { auto t = get(); t.y = v; set(t); } else y = v; }
    void setZ(float v) { if (get) { auto t = get(); t.z = v; set(t); } else z = v; }
    void setW(float v) { if (get) { auto t = get(); t.w = v; set(t); } else w = v; }

    Vector4D(Getter g, Setter s) : get(std::move(g)), set(std::move(s)) {}
};

void bindVector4D();