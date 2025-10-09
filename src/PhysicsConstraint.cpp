#include "PhysicsConstraint.h"

void PhysicsConstraint::setEnabled(bool v) {
	if (constraint) constraint->setEnabled(v);
}

void PhysicsConstraint::setEnableFeedback(bool v) {
	if (constraint) constraint->enableFeedback(v);
}

float PhysicsConstraint::getAppliedImpulse() {
	return constraint ? constraint->getAppliedImpulse() : 0;
}
