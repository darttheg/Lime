#pragma once

#include "irrBullet.h"
#include "Vector3D.h"
#include "StaticMesh.h"

#include "Compatible3D.h"

class RigidBody3D : public Compatible3D {
public:
    IRigidBody* rigidBody = nullptr;

    RigidBody3D(const StaticMesh& m);

    // Appearance
    // Controlled via StaticMesh

    // Physics
    void setFriction(float f);
    float getFriction();

    void setMass(float f);
    float getMass();

    void setDamping(float linear, float angular);

    // Apply forces

    irr::scene::ISceneNode* getNode() const override { return nullptr; }
};

void bindRigidBody3D();