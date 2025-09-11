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

		auto ba = colliderPair.find(bodyA);
		auto bb = colliderPair.find(bodyB);
		if (ba != colliderPair.end() && bb != colliderPair.end()) {
			int idA = ba->second->getNode()->getID();
			int idB = bb->second->getNode()->getID();

			if (physicsHandler->collisionsIgnoreSameID && idA == idB) continue;
		}

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
	Vector3D normalA;
	Vector3D normalB;
	float depth;

	for (const auto& pair : currentCollisions) {
		auto [a, b] = pair;

		auto bodyA = colliderPair.find(a);
		auto bodyB = colliderPair.find(b);
		if (bodyA == colliderPair.end() || bodyB == colliderPair.end()) continue;

		info = curData[a];
		posA = Vector3D(info.posA.x(), info.posA.y(), info.posA.z());
		posB = Vector3D(info.posB.x(), info.posB.y(), info.posB.z());
		normalA = Vector3D(-info.normalB.x(), -info.normalB.y(), -info.normalB.z());
		normalB = Vector3D(info.normalB.x(), info.normalB.y(), info.normalB.z());
		depth = info.depth;

		sol::table attrA;
		sol::table attrB;
		int idA = bodyA->second->getNode()->getID();
		int idB = bodyB->second->getNode()->getID();

		auto aA = irrHandler->comp3dmap.find(bodyA->second->getNode());
		if (aA != irrHandler->comp3dmap.end())
			attrA = aA->second;
		else
			attrA = sol::nil;

		auto bA = irrHandler->comp3dmap.find(bodyB->second->getNode());
		if (bA != irrHandler->comp3dmap.end())
			attrB = bA->second;
		else
			attrB = sol::nil;

		// Collision events: ID, attributes (nil if none), posX, depth, normal
		if (!lastCollisions.count(pair)) { // If was not colliding before, call OnEnter
			bodyA->second->getEnterEvent().get()->engineRun(idB, attrB, posB, normalB/*, depth*/);
			bodyB->second->getEnterEvent().get()->engineRun(idA, attrA, posA, normalA/*, depth*/);
		}
		else { // Else, OnInside
			bodyA->second->getInsideEvent().get()->engineRun(idB, attrB, posB, depth);
			bodyB->second->getInsideEvent().get()->engineRun(idA, attrA, posA, depth);
		}
	}

	for (const auto& pair : lastCollisions) {
		if (!currentCollisions.count(pair)) {
			auto [a, b] = pair;
			auto bodyA = colliderPair.find(a);
			auto bodyB = colliderPair.find(b);
			if (bodyA == colliderPair.end() || bodyB == colliderPair.end()) continue;

			sol::table attrA;
			sol::table attrB;
			int idA = bodyA->second->getNode()->getID();
			int idB = bodyB->second->getNode()->getID();

			auto aA = irrHandler->comp3dmap.find(bodyA->second->getNode());
			if (aA != irrHandler->comp3dmap.end())
				attrA = aA->second;
			else
				attrA = sol::nil;

			auto bA = irrHandler->comp3dmap.find(bodyB->second->getNode());
			if (bA != irrHandler->comp3dmap.end())
				attrB = bA->second;
			else
				attrB = sol::nil;

			bodyA->second->getExitEvent().get()->engineRun(idB, attrB, posB, normalB);
			bodyB->second->getExitEvent().get()->engineRun(idA, attrA, posA, normalA);
		}
	}

	lastCollisions.swap(currentCollisions);
	curData.clear();
}

void PhysicsHandler::setIgnoreSameID(bool v) {
	collisionsIgnoreSameID = v;
}
