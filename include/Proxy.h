#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"

#include <functional>
#include "IrrManagers.h"

// Proxy Vector objects to solve copies in binded Lime objects

struct Vector2DProxy {
    using Getter = std::function<Vector2D()>;
    using Setter = std::function<void(const Vector2D&)>;

    Getter get;
    Setter set;

    operator Vector2D() const { return get(); }

    float getX() const { return get().x; }
    float getY() const { return get().y; }

    void setX(float v) { auto t = get(); t.x = v; set(t); }
    void setY(float v) { auto t = get(); t.y = v; set(t); }
};

struct Vector3DProxy {
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

struct Vector4DProxy {
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

inline void bindProxyVectors() {
    (*lua).new_usertype<Vector2DProxy>("Vector2DProxy",
        "x", sol::property(&Vector2DProxy::getX, &Vector2DProxy::setX),
        "y", sol::property(&Vector2DProxy::getY, &Vector2DProxy::setY)
    );

    (*lua).new_usertype<Vector3DProxy>("Vector3DProxy",
        "x", sol::property(&Vector3DProxy::getX, &Vector3DProxy::setX),
        "y", sol::property(&Vector3DProxy::getY, &Vector3DProxy::setY),
        "z", sol::property(&Vector3DProxy::getZ, &Vector3DProxy::setZ)
    );

    (*lua).new_usertype<Vector4DProxy>("Vector4DProxy",
        "x", sol::property(&Vector4DProxy::getX, &Vector4DProxy::setX),
        "y", sol::property(&Vector4DProxy::getY, &Vector4DProxy::setY),
        "z", sol::property(&Vector4DProxy::getZ, &Vector4DProxy::setZ),
        "w", sol::property(&Vector4DProxy::getW, &Vector4DProxy::setW)
    );
}