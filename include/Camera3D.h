#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "DebugVisual.h"
#include "Material.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "LuaLime.h"
#include <string>
#include <vector>

#include "Compatible3D.h"

class Camera3D : public Compatible3D {
public:
    irr::scene::ICameraSceneNode* camera;
    irr::scene::ISceneNode* forwardChild;
    irr::scene::ISceneNode* leftChild;
    DebugSceneNode* d;

    Camera3D();
    Camera3D(const Camera3D& other);

    //~Camera3D() { destroy(); }

    irr::scene::ICameraSceneNode* createCamera();
    void destroy();

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);

    void setTargetBind(bool val);

    float getNearPlane();
    void setNearPlane(float val);

    float getFarPlane();
    void setFarPlane(float val);

    Vector2D getPlanes();
    void setPlanes(const Vector2D& planes);

    float getFOV();
    void setFOV(float val);

    bool getVisible();
    void setVisible(bool val);

    Vector3D getForward();
    Vector3D getLeft();
    Vector3D getUp();
    void setUp(const Vector3D& up);

    void setActive();

    float getAspect();
    void setAspect(float a);

    bool getOrtho();
    void setOrtho(bool val);

    bool getDebug();
    void setDebug(bool visible);

    void addToRenderQueue();

    irr::scene::ISceneNode* getNode() const override { return camera; }
};

void bindCamera3D();