#include "PhysicsHandler.h"
#include "IrrManagers.h"

PhysicsHandler::PhysicsHandler() {
	if (!device) return;
	
	world = createIrrBulletWorld(device, true, true);
	world->setDebugMode(m_debugMode);
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
			out = irrPhysicsDebugMode::EPDM_DrawAabb | irrPhysicsDebugMode::EPDM_DrawContactPoints | irrPhysicsDebugMode::EPDM_DrawWireframe;
	}

	m_debugMode = out;

	if (world)
		world->setDebugMode(m_debugMode);
}

void PhysicsHandler::setGravity(const Vector3D& g) {
	if (world)
		world->setGravity(irr::core::vector3df(g.x, g.y, g.z));
}

void PhysicsHandler::setStepFactor(float s) {
	m_stepMultiplier = s;
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

	world->stepSimulation(irrHandler->dt * m_stepMultiplier);
	world->debugDrawWorld(true);
	world->debugDrawProperties(drawProperties);
}