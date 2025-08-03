#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "LuaLime.h"
#include "DebugVisual.h"
#include <string>
#include <vector>

#include "Compatible3D.h"
#include "DrawSphere.h"

class Hitbox : public Compatible3D {
public:
    irr::scene::IMeshSceneNode* node = 0;
    irr::scene::ISceneNode* holder = 0;

    float radius;
    float height;
    int lod = 1; // 0 - Low poly, 1 - Medium, 2 - Highest (affects rings and sectors only, does not affect collision performance with other hitboxes)
    bool active = true;
    bool visible = false;
    bool collision = false;

    irr::scene::ISceneNode* getNode() const override { return holder; }

    Hitbox();
    Hitbox(float rad, float h);
    Hitbox(const Hitbox& other);

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);
    Vector3D getRotation();
    void setRotation(const Vector3D& rot);
    bool getActive();
    void setActive(bool active); // Active hitbox turns the hitbox yellow, otherwise blue (but if it is not visible then the capsule will not be rendered at all)
    bool getVisible();
    void setVisible(bool visible); // Sets vertex opacity to 255; hitboxes will always be truly visible
    int getLOD();
    void setLOD(int i); // Calls construct if LOD is different than lod member variable
    bool getCollision();
    void setCollision(bool enable); // If enabled, will be interactable with raypicks etc.
    int getID();
    void setID(int id);

    float getRadius();
    void setRadius(float r);
    float getHeight();
    void setHeight(float h);
    Vector2D getAttributes();
    void setAttributes(const Vector2D& att);

    void updateMaterial(bool updateOpacity, bool updateColor); // Update material based on attributes

    void construct();
    bool overlaps(const Hitbox& other);
    bool pointInside(const Vector3D& point);
    void destroy();
};

void bindHitbox();