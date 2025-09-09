#pragma once

#include "irrBullet.h"
#include "Vector3D.h"
#include "PhysicsObject.h"
#include <set>

class PhysicsHandler {
private:
private:
	float stepMultiplier = 1.0;
	irrPhysicsDebugMode debugMode = irrPhysicsDebugMode::EPDM_NoDebug;
public:
	PhysicsHandler();

	// Update
	void update();
	void onRender();

	// Gravity
	void setGravity(const Vector3D& g);

	// Step
	void setStepFactor(float s); // Multiplier (dt * s)
	void pause(bool p);
	bool isPaused();

	// Debug mode
	void setDebugMode(int m);
	void setDrawProperties(bool d) { drawProperties = d; };

	// Callbacks
	void handleCollisions();
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> lastCollisions;
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> currentCollisions;

	std::unordered_map<btCollisionObject*, PhysicsObject*> colliderPair;
public:
	irrBulletWorld* world = nullptr;
	bool drawProperties = false;
};