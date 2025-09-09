#include "RigidBody3D.h"

RigidBody3D::RigidBody3D(const StaticMesh& m, float mass) {
    auto shape = new IGImpactMeshShape(m.getNode(), m.meshNode->getMesh(), mass);

    // Create general creation function

    rigidBody = physicsHandler->world->addRigidBody(shape);
    rigidBody->includeNodeOnRemoval(false);
    mesh = m.meshNode;

    rigidBody->setSleepingThresholds(0.5, 0.5);

    PhysicsObject::createHandlerEntry();
}

RigidBody3D::RigidBody3D(const StaticMesh& m) : RigidBody3D(m, 0.0f) {
}

RigidBody3D::RigidBody3D(const StaticMesh& m, const StaticMesh& colliderMesh) {
    auto shape = new IGImpactMeshShape(m.getNode(), colliderMesh.meshNode->getMesh(), 0.0f);

    rigidBody = physicsHandler->world->addRigidBody(shape);
    rigidBody->includeNodeOnRemoval(false);
    mesh = m.meshNode;

    rigidBody->setSleepingThresholds(0.5, 0.5);

    PhysicsObject::createHandlerEntry();
}

bool RigidBody3D::destroy() {
    if (physicsHandler && physicsHandler->world) {
        physicsHandler->world->removeCollisionObject(rigidBody);
        PhysicsObject::removeHandlerEntry();
        return true;
    }
    return false;
}

Vector3D RigidBody3D::getPosition() {
    if (!rigidBody) return Vector3D();
    btVector3 pos = rigidBody->getPointer()->getWorldTransform().getOrigin();
    return Vector3D(pos.getX(), pos.getY(), pos.getZ());
}

void RigidBody3D::setPosition(const Vector3D& pos) {
    if (!rigidBody) return;
    
    btTransform transform = rigidBody->getPointer()->getWorldTransform();
    transform.getIdentity();
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    rigidBody->getPointer()->setWorldTransform(transform);
    rigidBody->getMotionState()->setWorldTransform(transform);
}

Vector3D RigidBody3D::getRotation() {
    if (!rigidBody) return Vector3D();

    btTransform transform = rigidBody->getPointer()->getWorldTransform();
    irr::core::matrix4 outMatrix;
    transform.getOpenGLMatrix(outMatrix.pointer());

    irr::core::vector3df rot = outMatrix.getRotationDegrees();
    return Vector3D(rot.X, rot.Y, rot.Z);
}

void RigidBody3D::setRotation(const Vector3D& rot) {
    if (!rigidBody) return;

    btTransform transform = rigidBody->getPointer()->getWorldTransform();
    irr::core::matrix4 inMatrix;
    inMatrix.setRotationDegrees(irr::core::vector3df(rot.x, rot.y, rot.z));

    const btVector3 ogPos = transform.getOrigin();

    irr::core::quaternion irrQuat;
    irrQuat.getMatrix(inMatrix);
    btQuaternion newRot(irrQuat.X, irrQuat.Y, irrQuat.Z, irrQuat.W);

    transform.setOrigin(ogPos);
    transform.setRotation(newRot);

    rigidBody->getPointer()->setWorldTransform(transform);
    if (btMotionState* ms = rigidBody->getMotionState())
        ms->setWorldTransform(transform);

    rigidBody->activate(true);
}

void RigidBody3D::setFriction(float f) {
    if (!rigidBody) return;
    rigidBody->setFriction(f);
}

float RigidBody3D::getFriction() {
    return rigidBody ? rigidBody->getFriction() : 0.0f;
}

void RigidBody3D::setAnisotropicFriction(const Vector3D& f) {
    if (!rigidBody) return;
    rigidBody->setAnisotropicFriction(irr::core::vector3df(f.x, f.y, f.z));
}

Vector3D RigidBody3D::getAnisotropicFriction() {
    return rigidBody ? Vector3D(rigidBody->getAnisotropicFriction().X, rigidBody->getAnisotropicFriction().Y, rigidBody->getAnisotropicFriction().Z) : Vector3D();
}

void RigidBody3D::setMass(float f) {
    if (!rigidBody) return;

    btVector3 inertia;
    btCollisionShape* shape = rigidBody->getPointer()->getCollisionShape();
    
    if (f > 0.0f)
        shape->calculateLocalInertia(f, inertia);

    rigidBody->getPointer()->setMassProps(f, inertia);
    rigidBody->getPointer()->updateInertiaTensor();
    rigidBody->getPointer()->activate(true);
}

float RigidBody3D::getMass() {
    return rigidBody ? 1.0 / rigidBody->getPointer()->getInvMass() : 0.0f;
}

float RigidBody3D::getLinearDamping() {
    return rigidBody ? rigidBody->getLinearDamping() : 0.0f;
}

void RigidBody3D::setLinearDamping(float f) {
    if (!rigidBody) return;
    rigidBody->setDamping(f, rigidBody->getAngularDamping());
}

float RigidBody3D::getAngularDamping() {
    return rigidBody ? rigidBody->getAngularDamping() : 0.0f;
}

void RigidBody3D::setAngularDamping(float f) {
    if (!rigidBody) return;
    rigidBody->setDamping(rigidBody->getLinearDamping(), f);
}

float RigidBody3D::getElasticity() {
    return rigidBody ? rigidBody->getRestitution() : 0.0f;
}

void RigidBody3D::setElasticity(float f) {
    if (!rigidBody) return;
    rigidBody->setRestitution(f);
}

Vector3D RigidBody3D::getCenterOfMass() {
    if (!rigidBody) return Vector3D();

    btVector3 out = rigidBody->getPointer()->getCenterOfMassTransform().getOrigin();
    return Vector3D(out.getX(), out.getY(), out.getZ());
}

void RigidBody3D::applyForce(const Vector3D& force, const Vector3D& relPos) {
    if (!rigidBody) return;

    rigidBody->applyForce(irr::core::vector3df(force.x, force.y, force.z), irr::core::vector3df(relPos.x, relPos.y, relPos.z));
}

void RigidBody3D::applyImpulse(const Vector3D& force, const Vector3D& relPos) {
    if (!rigidBody) return;

    rigidBody->applyImpulse(irr::core::vector3df(force.x, force.y, force.z), irr::core::vector3df(relPos.x, relPos.y, relPos.z));
}

void RigidBody3D::applyTorque(const Vector3D& force, bool impulsive) {
    if (!rigidBody) return;

    if (impulsive)
        rigidBody->applyTorqueImpulse(irr::core::vector3df(force.x, force.y, force.z));
    else
        rigidBody->applyTorque(irr::core::vector3df(force.x, force.y, force.z));
}

void RigidBody3D::faceTarget(const Vector3D& pos) {
    if (!rigidBody) return;
    rigidBody->faceTarget(irr::core::vector3df(pos.x, pos.y, pos.z));
}

void RigidBody3D::wakeUp() {
    if (!rigidBody) return;
    rigidBody->activate(true);
}

sol::table RigidBody3D::getMeshAttributes() {
    if (!mesh) return lua->create_table();

    return mesh ? irrHandler->comp3dmap[mesh] : lua->create_table();
}

void RigidBody3D::clearForces() {
    if (!rigidBody) return;
    rigidBody->getPointer()->clearForces();
}

void RigidBody3D::applySimpleImpulse(float forceMagnitude, const Vector3D& worldPos) {
    if (!rigidBody) return;

    btVector3 centerMass = rigidBody->getPointer()->getWorldTransform().getOrigin();
    btVector3 dir = btVector3(worldPos.x, worldPos.y, worldPos.z) - centerMass;
    dir.normalize();

    btVector3 imp = dir * -1.0 * forceMagnitude;
    btVector3 rel = btVector3(worldPos.x, worldPos.y, worldPos.z) - centerMass;

    rigidBody->getPointer()->applyImpulse(imp, rel);
    rigidBody->activate(true);
}

void RigidBody3D::applySimpleForce(float forceMagnitude, const Vector3D& worldPos) {
    if (!rigidBody) return;

    btVector3 centerMass = rigidBody->getPointer()->getWorldTransform().getOrigin();
    btVector3 dir = btVector3(worldPos.x, worldPos.y, worldPos.z) - centerMass;
    dir.normalize();

    btVector3 imp = dir * -1.0 * forceMagnitude;
    btVector3 rel = btVector3(worldPos.x, worldPos.y, worldPos.z) - centerMass;

    rigidBody->getPointer()->applyForce(imp, rel);
    rigidBody->activate(true);
}

Vector3D RigidBody3D::getLinearVelocity() {
    return rigidBody ? Vector3D(rigidBody->getLinearVelocity().X, rigidBody->getLinearVelocity().Y, rigidBody->getLinearVelocity().Z) : Vector3D();
}

void RigidBody3D::setLinearVelocity(const Vector3D& vel) {
    if (!rigidBody) return;
    rigidBody->setLinearVelocity(irr::core::vector3df(vel.x, vel.y, vel.z));
}

Vector3D RigidBody3D::getAngularVelocity() {
    return rigidBody ? Vector3D(rigidBody->getAngularVelocity().X, rigidBody->getAngularVelocity().Y, rigidBody->getAngularVelocity().Z) : Vector3D();
}

void RigidBody3D::setAngularVelocity(const Vector3D& vel) {
    if (!rigidBody) return;
    rigidBody->setAngularVelocity(irr::core::vector3df(vel.x, vel.y, vel.z));
}

Vector3D RigidBody3D::getLinearFactor() {
    return rigidBody ? Vector3D(rigidBody->getLinearFactor().X, rigidBody->getLinearFactor().Y, rigidBody->getLinearFactor().Z) : Vector3D();
}

void RigidBody3D::setLinearFactor(const Vector3D& f) {
    if (!rigidBody) return;
    rigidBody->setLinearFactor(irr::core::vector3df(f.x, f.y, f.z));
}

Vector3D RigidBody3D::getAngularFactor() {
    return rigidBody ? Vector3D(rigidBody->getAngularFactor().X, rigidBody->getAngularFactor().Y, rigidBody->getAngularFactor().Z) : Vector3D();
}

void RigidBody3D::setAngularFactor(const Vector3D& f) {
    if (!rigidBody) return;
    rigidBody->setAngularFactor(irr::core::vector3df(f.x, f.y, f.z));
}

Vector3D RigidBody3D::getForward() {
    if (!rigidBody) return Vector3D(0, 0, 1);

    btVector3 f = btVector3(0, 0, 1);
    f = rigidBody->getPointer()->getWorldTransform().getBasis() * f;
    return Vector3D(f.getX(), f.getY(), f.getZ());
}

float RigidBody3D::getSleepingThreshold() {
    return rigidBody ? rigidBody->getLinearSleepingThreshold() : 0.0f;
}

void RigidBody3D::setSleepingThreshold(float f) { // Velocity vector must be <= f to sleep (linear and angular)
    if (!rigidBody) return;

    if (f <= 0)
        rigidBody->setActivationState(EActivationState::EAS_DISABLE_DEACTIVATION);
    else
        rigidBody->setActivationState(EActivationState::EAS_WANTS_DEACTIVATION);

    rigidBody->setSleepingThresholds(f, f);
}

void RigidBody3D::setGhost(bool v) {
    if (!rigidBody) return;

    rigidBody->setCollisionFlags(v ? ECollisionFlag::ECF_NO_CONTACT_RESPONSE : (ECollisionFlag)0);
}

#include "Proxy.h"
void bindRigidBody3D() {
    sol::usertype<RigidBody3D> bindType = lua->new_usertype<RigidBody3D>("RigidBody3D",
        sol::constructors<RigidBody3D(const StaticMesh& m, float mass), RigidBody3D(const StaticMesh& m), RigidBody3D(const StaticMesh& m, const StaticMesh& colliderMesh)>(),

        sol::base_classes, sol::bases<Compatible3D, PhysicsObject>(),

        "position", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setPosition(v); }
        ),
        "rotation", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getRotation(); }, [&](auto v) { c.setRotation(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setRotation(v); }
        ),

        "friction", sol::property(&RigidBody3D::getFriction, &RigidBody3D::setFriction),
        "anisotropicFriction", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getAnisotropicFriction(); }, [&](auto v) { c.setAnisotropicFriction(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setAnisotropicFriction(v); }
        ),

        "mass", sol::property(&RigidBody3D::getMass, &RigidBody3D::setMass),
        "linearDamping", sol::property(&RigidBody3D::getLinearDamping, &RigidBody3D::setLinearDamping),
        "angularDamping", sol::property(&RigidBody3D::getAngularDamping, &RigidBody3D::setAngularDamping),
        "elasticity", sol::property(&RigidBody3D::getElasticity, &RigidBody3D::setElasticity),
        "sleepThreshold", sol::property(&RigidBody3D::getSleepingThreshold, &RigidBody3D::setSleepingThreshold),

        "linearVelocity", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getLinearVelocity(); }, [&](auto v) { c.setLinearVelocity(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setLinearVelocity(v); }
        ),
        "angularVelocity", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getAngularVelocity(); }, [&](auto v) { c.setAngularVelocity(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setAngularVelocity(v); }
        ),
        "linearFactor", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getLinearFactor(); }, [&](auto v) { c.setLinearFactor(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setLinearFactor(v); }
        ),
        "angularFactor", sol::property(
            [](RigidBody3D& c) { return Vector3DProxy{ [&] { return c.getAngularFactor(); }, [&](auto v) { c.setAngularFactor(v); } }; },
            [](RigidBody3D& c, const Vector3D& v) { c.setAngularFactor(v); }
        )
    );

    bindType["destroy"] = &RigidBody3D::destroy;
    bindType["clearForces"] = &RigidBody3D::clearForces;
    bindType["getCenterOfMass"] = &RigidBody3D::getCenterOfMass;
    bindType["wakeUp"] = &RigidBody3D::wakeUp;
    bindType["lookAt"] = &RigidBody3D::faceTarget;
    bindType["getForward"] = &RigidBody3D::getForward;

    bindType["applyForceRelative"] = &RigidBody3D::applyForce;
    bindType["applyImpulseRelative"] = &RigidBody3D::applyImpulse;    
    bindType["applyImpulse"] = &RigidBody3D::applySimpleImpulse;
    bindType["applyForce"] = &RigidBody3D::applySimpleForce;
    bindType["applyTorque"] = &RigidBody3D::applyTorque;

    bindType["setGhost"] = &RigidBody3D::setGhost;

    bindType["getMeshAttributes"] = &RigidBody3D::getMeshAttributes; // From irrcomp3dmap
}
