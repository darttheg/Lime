#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "DrawSphere.h"
#include <string>
#include <vector>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

// Enum for debug types
enum class DebugType {
    CAMERA = 0,
    RAY_PICK,
    LIGHT,
    PARTICLE_SYSTEM,
    EMPTY
};

class DebugSceneNode : public ISceneNode {
public:
    // Constructor
    DebugSceneNode(ISceneNode* parent, ISceneManager* smgr, s32 id, DebugType type)
        : ISceneNode(parent, smgr, id), debugType(type) {
    }

    virtual void OnRegisterSceneNode() override {
        SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
        ISceneNode::OnRegisterSceneNode();
    }

    virtual void render() override {
        switch (debugType) {
        case DebugType::CAMERA:
            renderCameraDebug();
            break;
        case DebugType::RAY_PICK:
            renderRayPickDebug();
            break;
        case DebugType::LIGHT:
            renderLightDebug();
            break;
        case DebugType::PARTICLE_SYSTEM:
            renderParticleDebug();
            break;
        case DebugType::EMPTY:
            renderEmptyDebug();
            break;
        }
    }

    // Set the debug type
    void setDebugType(DebugType type) {
        debugType = type;
    }

    // Access bounding box (required override)
    virtual const aabbox3d<f32>& getBoundingBox() const override {
        static aabbox3d<f32> dummyBox(vector3df(-1, -1, -1), vector3df(1, 1, 1));
        return dummyBox;
    }

    virtual u32 getMaterialCount() const override {
        return 0;
    }

    virtual SMaterial& getMaterial(u32 i) override {
        static SMaterial dummyMaterial;
        return dummyMaterial;
    }

    DebugType debugType;
    
    Vector3D raypick_start;
    Vector3D raypick_end;
    bool raypick_hit;
    float raypick_life;

    float rad;
    vector3df pos;
    int val1;
    int val2;
    SColor col;

private:
    void renderCameraDebug() {
        float boxSize = 0.25;

        vector3df nodePosition = getAbsolutePosition();

        const aabbox3d<f32> box(
            nodePosition - vector3df(boxSize, boxSize, boxSize),
            nodePosition + vector3df(boxSize, boxSize, boxSize)
        );

        SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->setTransform(video::ETS_WORLD, core::matrix4());

        driver->draw3DBox(box, SColor(255, 255, 0, 0));

        vector3df start = nodePosition;
        vector3df direction = getParent()->getRotation().rotationToDirection();
        vector3df end = nodePosition + direction * 2;

        driver->draw3DLine(start, end, SColor(255, 255, 0, 0));
    }

    void renderRayPickDebug() {
        SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->setTransform(video::ETS_WORLD, core::matrix4());

        vector3df end = vector3df(raypick_end.x, raypick_end.y, raypick_end.z);

        driver->draw3DLine(
            vector3df(raypick_start.x, raypick_start.y, raypick_start.z),
            end,
            SColor(255, 255 * !raypick_hit, 255 * raypick_hit, 0)
        );

        if (raypick_hit) {
            float boxSize = 0.05;

            const aabbox3d<f32> box(
                end - vector3df(boxSize, boxSize, boxSize),
                end + vector3df(boxSize, boxSize, boxSize)
            );

            driver->draw3DBox(box, SColor(255, 255 * !raypick_hit, 255 * raypick_hit, 0));
        }

        raypick_life -= irrHandler->dt;

        if (raypick_life <= 0)
            this->remove();
    }

    void renderLightDebug() {
        SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->setTransform(video::ETS_WORLD, core::matrix4());

        float boxSize = 0.125;

        vector3df nodePosition = getAbsolutePosition();

        aabbox3d<f32> box(
            nodePosition - vector3df(boxSize, boxSize, boxSize),
            nodePosition + vector3df(boxSize, boxSize, boxSize)
        );

        driver->draw3DBox(box, SColor(255, 255, 255, 0));

        vector3df start = nodePosition;
        vector3df direction = getParent()->getRotation().rotationToDirection();
        vector3df end = nodePosition + direction * 1;

        driver->draw3DLine(start, end, SColor(255, 255, 255, 0));

        // val1 == is point light
        if (rad > 0.0f && val1 == 1)
            renderSphere(start, rad, 0, 10, 10, SColor(255, 255, 255, 0));

    }

    void renderParticleDebug() {
        SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->setTransform(video::ETS_WORLD, core::matrix4());

        float boxSize = 0.25;

        vector3df nodePosition = getAbsolutePosition();

        const aabbox3d<f32> box(
            nodePosition - vector3df(boxSize, boxSize, boxSize),
            nodePosition + vector3df(boxSize, boxSize, boxSize)
        );

        driver->draw3DBox(box, SColor(255, 160, 30, 240));
    }

    void renderEmptyDebug() {
        SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->setTransform(video::ETS_WORLD, core::matrix4());

        float boxSize = 0.125;

        vector3df nodePosition = getAbsolutePosition();

        const aabbox3d<f32> box(
            nodePosition - vector3df(boxSize, boxSize, boxSize),
            nodePosition + vector3df(boxSize, boxSize, boxSize)
        );

        driver->draw3DBox(box, SColor(255, 255, 255, 255));
    }
};
