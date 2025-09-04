#include "RigidBody3D.h"

RigidBody3D::RigidBody3D(const StaticMesh& m, float mass) {
    auto shape = new IGImpactMeshShape(m.getNode(), m.meshNode->getMesh(), mass);

    rigidBody = physicsHandler->world->addRigidBody(shape);
    rigidBody->includeNodeOnRemoval(false);

    /*
    rigidBody->applyForce(); // Influences velocity, flag for central
    rigidBody->applyImpulse(); // Instant, does not influence velocity, flag for central
    rigidBody->applyTorque(); // Rotational, flag for impulse
    rigidBody->faceTarget(); // Points object in direction
    rigidBody->clearForces(); // Clear forces
    rigidBody->setLinearVelocity(); // Set linear velocity
    rigidBody->setAngularVelocity(); // Set angular velocity
    rigidBody->setSleepingThresholds(); // Set sleep thresholds (linear, angular)

    Add affectors
    Set position, rotation, scale
    */
}

RigidBody3D::RigidBody3D(const StaticMesh& m) : RigidBody3D(m, 0.0f) {
}

bool RigidBody3D::destroy() {
    if (physicsHandler && physicsHandler->world) {
        physicsHandler->world->removeCollisionObject(rigidBody);
        return true;
    }
    return false;
}

void bindRigidBody3D() {
    sol::usertype<RigidBody3D> bindType = lua->new_usertype<RigidBody3D>("RigidBody3D",
        sol::constructors<RigidBody3D(const StaticMesh& m, float mass), RigidBody3D(const StaticMesh& m)>(),

        sol::base_classes, sol::bases<Compatible3D>()
    );

    bindType["destroy"] = &RigidBody3D::destroy;
}