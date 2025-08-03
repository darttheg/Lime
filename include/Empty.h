#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Material.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "LuaLime.h"
#include <string>
#include <vector>
#include "DebugVisual.h"

#include "Compatible3D.h"

using namespace irr;
using namespace video;

class Empty : public Compatible3D {
public:
    irr::scene::ISceneNode* emp = nullptr;
    DebugSceneNode* d = nullptr;

    Empty();
    Empty(const Vector3D& pos);
    Empty(const Vector3D& pos, const Vector3D& rot);
    Empty(const Vector3D& pos, const Vector3D& rot, const Vector3D& scale);

    bool getVisibility() const;
    void setVisibility(bool visible);

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);

    Vector3D getScale();
    void setScale(const Vector3D& scale);

    bool getDebug();
    void setDebug(bool visible);

    sol::table getBoundingBox();

    void destroy();

    irr::scene::ISceneNode* getNode() const override { return emp; }
};

void bindEmpty();