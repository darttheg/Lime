#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

class Compatible3D {
public:
    virtual ~Compatible3D() = default;

    bool hasEntry = false;

    virtual irr::scene::ISceneNode* getNode() const = 0;
    void setParent(sol::optional<Compatible3D*> parent);
    Vector3D getAbsPos();
    Vector3D getAbsRot();
    Vector3D getAbsScale();
    void updateAbsPos();
    sol::table getEntry();
    void setEntry(sol::table obj);
    void createEntry();
    void destroyEntry();

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);
    Vector3D getScale();
    void setScale(const Vector3D& scale);
    bool getVisibility() const;
    void setVisibility(bool v);

    int getID();
    void setID(int i);
};

void bindCompatible3D();