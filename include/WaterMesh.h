#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "StaticMesh.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Material.h"
#include <string>

#include "Compatible3D.h"
#include "SolConversions.h"

class Water : public Compatible3D {
public:
    irr::scene::ISceneNode* water;
    float height;
    float speed;
    float length;
    irr::core::vector2df tileSize;
    irr::core::vector2df tileCount;
    irr::core::vector2df texRepeat;
    irr::video::SMaterial material;
    irr::scene::IMesh* rawMesh;
    int shadow;
    bool hadShadow;

    Water(float h, float s, float l, const Vector2D& ts, const Vector2D& tc, const Vector2D& tr, const Material& m);
    Water();
    Water(const Material& m);
    Water(const Vector2D& ts, const Vector2D& tc, const Vector2D& tr);
    Water(sol::table tbl);

    void refreshMesh();
    void createRaw();
    void destroy();

    float getHeight();
    void setHeight(float i);

    float getSpeed();
    void setSpeed(float i);

    float getLength();
    void setLength(float i);;

    void loadMaterial(const Material& m);

    Vector2D getTileSize();
    void setTileSize(const Vector2D& other);

    Vector2D getTileCount();
    void setTileCount(const Vector2D& other);

    Vector2D getTexRepeat();
    void setTexRepeat(const Vector2D& other);

    irr::scene::ISceneNode* getNode() const override { return water; }
};

void bindWater();