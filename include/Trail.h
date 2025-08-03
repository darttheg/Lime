#pragma once

#include "GhostTrailSceneNode.h"
#include "IrrManagers.h"
#include "Vector2D.h"
#include "Texture.h"
#include "StaticMesh.h"
#include <string>
#include <vector>

#include "Compatible3D.h"

class Trail : public Compatible3D {
public:
    ghostTrailSceneNode* t = nullptr;
    irr::scene::ISceneNode* empty;
    float trailWidth;
    int segments;
    Vector3D wind;
    int shadow;
    bool hadShadow;
    int mode;

    Trail();

    void make();
    void destroy();

    bool getVisibility() const;
    void setVisibility(bool visible);

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);

    Vector3D getScale();
    void setScale(const Vector3D& scale);

    float getWidth();
    void setWidth(float w);

    int getSegments();
    void setSegments(int s);

    Vector3D getWind();
    void setWind(const Vector3D& w);

    bool getDebug();
    void setDebug(bool visible);

    bool loadMaterial(const Material& material);

    void exclude();
    int getShadows();
    void setShadows(int i);

    void setUpdateNormals(bool enable);

    int getAlignmentMode();
    void setAlignmentMode(int m);

    float getFixedSize();
    void setFixedSize(float s);

    irr::scene::ISceneNode* getNode() const override { return empty; }
};

void bindTrail();