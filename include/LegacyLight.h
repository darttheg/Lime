#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "LuaLime.h"
#include "DebugVisual.h"
#include <string>
#include <vector>

#include "Compatible3D.h"

class LegacyLight : public Compatible3D {
public:
    irr::scene::ILightSceneNode* light = 0;
    DebugSceneNode* d = nullptr;

    irr::scene::ISceneNode* getNode() const override { return light; }

    LegacyLight(const Vector3D& pos, const Vector3D& rot, int type, const Vector4D& color);
    LegacyLight(int type);
    LegacyLight(const Vector3D& pos);
    LegacyLight();
    LegacyLight(const LegacyLight& other);
    Vector4D getLightColor();
    void setLightColor(const Vector4D& col);
    Vector4D getAmbientColor();
    void setAmbientColor(const Vector4D& col);
    Vector4D getSpecColor();
    void setSpecColor(const Vector4D& col);
    int getType();
    void setType(int i);
    Vector2D getCones();
    void setCones(const Vector2D& cones);
    float getRadius();
    void setRadius(float r);
    Vector3D getAttenuation();
    void setAttenuation(const Vector3D& a);
    float getFalloff();
    void setFalloff(float f);
    void destroy();
    bool getDebug();
    void setDebug(bool visible);
};

void bindLegacyLight();