#pragma once

#include "irrBullet.h"
#include "Vector3D.h"

class PhysicsHandler {
private:
private:
	float m_stepMultiplier = 1.0;
public:
	PhysicsHandler();

	// Update
	void update();

	// Gravity
	void setGravity(const Vector3D& g);
	float getGravity();

	// Step
	void setStep(float s); // Multiplier (dt * s)

	// Debug mode
	void setDebugMode(int m);
public:
	irrBulletWorld* world = nullptr;
};