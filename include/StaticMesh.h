#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Material.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "LuaLime.h"
#include <string>
#include <vector>

#include "Compatible3D.h"
#include "MeshBuffer.h"

using namespace irr;
using namespace video;

class StaticMesh : public Compatible3D {
public:
    irr::scene::IAnimatedMeshSceneNode* meshNode;
    std::string meshPath;
    irr::scene::ITriangleSelector* selector;
    bool collisionEnabled;
    irr::video::SColor vColor;
    int shadow;
    bool hadShadow;

    StaticMesh();
    StaticMesh(const std::string& filePath);
    StaticMesh(const StaticMesh& other);
    StaticMesh(irr::scene::IAnimatedMeshSceneNode* node);

    std::string getMesh() const;

    bool loadMesh(const std::string& filePath);
    bool loadMeshWithTangents(const std::string& filePath);
    bool fullLoadMesh(const std::string& filePath, bool doTangents);
    bool loadMeshViaBuffer(const MeshBuffer& b);

    int getShadows();
    void setShadows(int i);

    unsigned int getVertexCount() const;
    unsigned int getMaterialCount() const;

    void deload();
    bool loadMaterial(const Material& material, int slot);

    bool getCollision() const;
    void setCollision(bool enable);

    void exclude();
    bool getVisibility() const;
    void setVisibility(bool visible);

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);

    Vector3D getScale();
    void setScale(const Vector3D& scale);

    int getID();
    void setID(int i);

    int getFrame();
    void setFrame(int i);
    int getFrameCount();

    sol::table getBoneData(irr::scene::IBoneSceneNode* bone);
    sol::table getBoneInfoByIndex(int i);
    sol::table getBoneInfoByName(const std::string& name);

    void normalizeNormals(bool enable);
    bool getDebug();
    void setDebug(bool visible);

    sol::table getBoundingBox();
    void recalculateBoundingBox();
    void makePlanarMapping();
    void setHardwareHint(int i);

    Vector4D getVColor();
    void setVColor(const Vector4D& col);

    bool writeToFile(std::string path);

    void setAutomaticCulling(bool enable);

    irr::scene::ISceneNode* getNode() const override { return meshNode; }
};

void bindStaticMesh();