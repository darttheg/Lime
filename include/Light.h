#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "StaticMesh.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "LuaLime.h"
#include "DebugVisual.h"
#include <string>
#include <vector>

#include "Compatible3D.h"

class Light : public Compatible3D {
public:
    int index;
    irr::scene::ISceneNode* target;
    irr::scene::ISceneNode* holder;
    DebugSceneNode* d = nullptr;

    Light();
    Light(const Vector3D& pos, const Vector3D& rot, const Vector4D& col);
    Light(const Vector3D& pos, const Vector3D& rot, const Vector4D& col, const Vector2D& viewPlanes, float fov, bool directional);

    void createBase();
    void updateTarget();

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);

    Vector4D getColor();
    void setColor(const Vector4D& col);

    bool getDirectional();
    void setDirectional(bool enable);

    Vector2D getViewPlanes();
    void setViewPlanes(const Vector2D& planes);

    float getFOV();
    void setFOV(float f);

    bool getActive();
    void setActive(bool enable);

    void destroy();

    bool getDebug();
    void setDebug(bool visible);

    irr::scene::ISceneNode* getNode() const override { return holder; }
};

void bindLight();