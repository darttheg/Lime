#include "PhysicsHandler.h"
#include "IrrManagers.h"

PhysicsHandler::PhysicsHandler() {
	if (!device) return;
	
	world = createIrrBulletWorld(device, true, true);
	world->setDebugMode(debugMode);
	world->setGravity(irr::core::vector3df(0, -9.8, 0));
}

void PhysicsHandler::setDebugMode(int m) {
	irrPhysicsDebugMode out = irrPhysicsDebugMode::EPDM_NoDebug;

	switch (m) {
		case 1:
			out = irrPhysicsDebugMode::EPDM_DrawAabb;
			break;
		case 2:
			out = irrPhysicsDebugMode::EPDM_DrawContactPoints;
			break;
		case 3:
			out = irrPhysicsDebugMode::EPDM_DrawWireframe; // EPDM_FastWireframe?
			break;
		case 4:
			out = irrPhysicsDebugMode::EPDM_DrawConstraints | irrPhysicsDebugMode::EPDM_DrawConstraintLimits;
		case 5:
			out = irrPhysicsDebugMode::EPDM_DrawAabb | irrPhysicsDebugMode::EPDM_DrawContactPoints | irrPhysicsDebugMode::EPDM_DrawWireframe |
				  irrPhysicsDebugMode::EPDM_DrawConstraints | irrPhysicsDebugMode::EPDM_DrawConstraintLimits;
	}

	debugMode = out;

	if (world)
		world->setDebugMode(debugMode);
}

void PhysicsHandler::setGravity(const Vector3D& g) {
	if (world)
		world->setGravity(irr::core::vector3df(g.x, g.y, g.z));
}

void PhysicsHandler::setStepFactor(float s) {
	stepMultiplier = s;
}

void PhysicsHandler::pause(bool p) {
	if (world)
		world->pauseSimulation(p);
}

bool PhysicsHandler::isPaused() {
	return world ? world->simulationPaused() : false;
}

void PhysicsHandler::update() {
	if (!world) return;

	world->stepSimulation(irrHandler->dt * stepMultiplier);
}

void PhysicsHandler::onRender() {
	if (!world) return;

	handleCollisions();

	SMaterial m;
	m.Lighting = false;
	driver->setMaterial(m);
	driver->setTransform(video::ETS_WORLD, core::matrix4());
	world->debugDrawWorld();
}

void PhysicsHandler::handleCollisions() {
	btDispatcher* d = world->getPointer()->getDispatcher();

	currentCollisions.clear();
	curData.clear();

	for (int i = 0; i < d->getNumManifolds(); ++i) {
		btPersistentManifold* m = d->getManifoldByIndexInternal(i);
		if (m->getNumContacts() == 0) continue; // No collisions

		const btManifoldPoint* closest = nullptr;
		btScalar lowestDist = BT_LARGE_FLOAT;
		for (int j = 0; j < m->getNumContacts(); ++j) {
			btManifoldPoint& cur = m->getContactPoint(j);
			if (cur.getDistance() <= 0 && cur.getDistance() < lowestDist) {
				lowestDist = cur.getDistance();
				closest = &cur;
			}
		}
				
		if (lowestDist > 0 || !closest) continue;
		
		auto bodyA = const_cast<btCollisionObject*>(m->getBody0());
		auto bodyB = const_cast<btCollisionObject*>(m->getBody1());

		if (bodyA > bodyB) std::swap(bodyA, bodyB);
		currentCollisions.insert({ bodyA, bodyB });

		ContactInfo c;
		c.depth = lowestDist;
		c.posA = closest->getPositionWorldOnA();
		c.posB = closest->getPositionWorldOnB();
		c.normalB = closest->m_normalWorldOnB;
		curData.insert({ bodyA, c });
	}

	ContactInfo info;
	Vector3D posA;
	Vector3D posB;
	Vector3D normal;
	float depth;

	for (const auto& pair : currentCollisions) {
		auto [a, b] = pair;

		auto bodyA = colliderPair.find(a);
		auto bodyB = colliderPair.find(b);
		if (bodyA == colliderPair.end() || bodyB == colliderPair.end()) continue;

		info = curData[a];
		posA = Vector3D(info.posA.x(), info.posA.y(), info.posA.z());
		posB = Vector3D(info.posB.x(), info.posB.y(), info.posB.z());
		normal = Vector3D(info.normalB.x(), info.normalB.y(), info.normalB.z());
		depth = info.depth;

		if (!lastCollisions.count(pair)) { // If was not colliding before, call OnEnter
			bodyA->second->getEnterEvent().get()->engineRun(bodyB, posB, depth, normal);
			bodyB->second->getEnterEvent().get()->engineRun(bodyA, posA, depth, normal);
		}
		else { // Else, OnInside
			bodyA->second->getInsideEvent().get()->engineRun(bodyB, posB, depth);
			bodyB->second->getInsideEvent().get()->engineRun(bodyA, posA, depth);
		}
	}

	for (const auto& pair : lastCollisions) {
		if (!currentCollisions.count(pair)) {
			auto [a, b] = pair;
			auto bodyA = colliderPair.find(a);
			auto bodyB = colliderPair.find(b);
			if (bodyA == colliderPair.end() || bodyB == colliderPair.end()) continue;

			bodyA->second->getExitEvent().get()->engineRun(bodyB, posB, normal);
			bodyB->second->getExitEvent().get()->engineRun(bodyA, posA, normal);
		}
	}

	lastCollisions.swap(currentCollisions);
	curData.clear();
}