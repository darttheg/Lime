#pragma once

#include "irrBullet.h"
#include "Vector3D.h"

class PhysicsHandler {
private:
private:
	float m_stepMultiplier = 1.0;
	irrPhysicsDebugMode m_debugMode = irrPhysicsDebugMode::EPDM_NoDebug;
public:
	PhysicsHandler();

	// Update
	void update();

	// Gravity
	void setGravity(const Vector3D& g);

	// Step
	void setStepFactor(float s); // Multiplier (dt * s)
	void pause(bool p);
	bool isPaused();

	// Debug mode
	void setDebugMode(int m);
	void setDrawProperties(bool d) { drawProperties = d; };
public:
	irrBulletWorld* world = nullptr;
	bool drawProperties = false;
};