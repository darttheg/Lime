#include "PhysicsObject.h"
#include "IrrManagers.h"

void PhysicsObject::createHandlerEntry() {
	btCollisionObject* out = getCollisionObject();
	if (!out) return;

	physicsHandler->colliderPair[out] = this;
}

void PhysicsObject::removeHandlerEntry() {
	btCollisionObject* out = getCollisionObject();
	if (!out) return;

	physicsHandler->colliderPair.erase(out);
}

void bindPhysicsObject() {
	sol::usertype<PhysicsObject> bindType = lua->new_usertype<PhysicsObject>("PhysicsObject"
	);

	bindType["OnEnter"] = sol::property([](PhysicsObject& self) { return self.getEnterEvent(); });
	bindType["OnInside"] = sol::property([](PhysicsObject& self) { return self.getInsideEvent(); });
	bindType["OnExit"] = sol::property([](PhysicsObject& self) { return self.getExitEvent(); });
}
