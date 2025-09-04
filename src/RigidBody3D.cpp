#include "RigidBody3D.h"

RigidBody3D::RigidBody3D(const StaticMesh& m) {
    auto shape = new IBoxShape(m.getNode(), 1.0f, false);

    rigidBody = physicsHandler->world->addRigidBody(shape);

    rigidBody->applyForce(); // Influences velocity, flag for central
    rigidBody->applyImpulse(); // Instant, does not influence velocity, flag for central
    rigidBody->applyTorque(); // Rotational, flag for impulse
    rigidBody->faceTarget(); // Points object in direction
    rigidBody->clearForces(); // Clear forces
    rigidBody->setLinearVelocity(); // Set linear velocity
    rigidBody->setAngularVelocity(); // Set angular velocity
}

void bindRigidBody3D() {
    sol::usertype<RigidBody3D> bindType = lua->new_usertype<RigidBody3D>("RigidBody3D",
        sol::constructors<RigidBody3D(const StaticMesh& m)>(),

        sol::base_classes, sol::bases<Compatible3D>()
    );
}