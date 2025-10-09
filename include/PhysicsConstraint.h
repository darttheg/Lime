#pragma once

#include "IrrManagers.h"

class PhysicsConstraint {
public:
	btTypedConstraint* constraint = nullptr;
	bool doCollisionWithLinkedBodies = false;

	void setEnabled(bool v);
	void setEnableFeedback(bool v);
	float getAppliedImpulse();

	void destroy();
};

class ConstraintHinge : public PhysicsConstraint {
public:
	ConstraintHinge();

};

class ConstraintSlider : public PhysicsConstraint {
public:

};

class ConstraintConeTwist : public PhysicsConstraint {
public:

};

class ConstraintGeneric6Dof : public PhysicsConstraint {
public:

};

class ConstraintGeneric6DofSpring2 : public PhysicsConstraint {
public:

};

class ConstraintFixed : public PhysicsConstraint {
public:

};

class ConstraintGear : public PhysicsConstraint {
public:

};

void bindPhysicsConstraint();