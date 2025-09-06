#pragma once

#include "irrBullet.h"
#include "Vector3D.h"
#include "StaticMesh.h"

class RigidBody3D {
private:
    irr::scene::ISceneNode* mesh = nullptr;
public:
    IRigidBody* rigidBody = nullptr;

    RigidBody3D(const StaticMesh& m);
    RigidBody3D(const StaticMesh& m, const StaticMesh& colliderMesh);
    // RigidBody3D(const MeshBuffer& m); ?

    RigidBody3D(const StaticMesh& m, float mass); // Basic testing constructor if you just want to get something in real quick

    bool destroy();

    // General

    Vector3D getPosition();
    void setPosition(const Vector3D& pos);

    Vector3D getRotation();
    void setRotation(const Vector3D& rot);

    // No scalar operations. Resize your mesh before attaching it or make a proper collision shape.
    /*Vector3D getScale();
    void setScale(const Vector3D& scale) const;*/

    // Physics attributes

    void setFriction(float f);
    float getFriction();
    void setAnisotropicFriction(const Vector3D& f);
    Vector3D getAnisotropicFriction();

    void setMass(float f);
    float getMass();

    float getLinearDamping();
    void setLinearDamping(float f);
    float getAngularDamping();
    void setAngularDamping(float f);

    float getElasticity();
    void setElasticity(float f); // setRestitution

    // Apply forces
    
    Vector3D getCenterOfMass();
    // relativePosition is actually in world-space coordinates...
    void applyForce(const Vector3D& force, const Vector3D& relativePosition);
    void applyImpulse(const Vector3D& force, const Vector3D& relativePosition);
    void applyTorque(const Vector3D& force, bool impulsive);
    void faceTarget(const Vector3D& position);
    void clearForces();

    void applySimpleImpulse(float force, const Vector3D& worldPos);

    Vector3D getLinearVelocity();
    void setLinearVelocity(const Vector3D& vel);
    Vector3D getAngularVelocity();
    void setAngularVelocity(const Vector3D& vel);
    Vector3D getLinearFactor();
    void setLinearFactor(const Vector3D& f);
    Vector3D getAngularFactor();
    void setAngularFactor(const Vector3D& f);

    // void setSleepingThresholds(float linear, float angular);
    float getSleepingThreshold();
    void setSleepingThreshold(float f);
    void setCollisionShape(const StaticMesh& m);
    void setCollisionShape(const MeshBuffer& m);
    Vector3D getForward();

    void wakeUp(); // Wakes up sleeping object

    sol::table getMeshAttributes();
};

void bindRigidBody3D();