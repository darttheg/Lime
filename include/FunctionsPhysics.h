#include "FunctionsImports.h"

namespace Bind {
	void setDebugMode(int m) {
		if (!physicsHandler) return;
		physicsHandler->setDebugMode(m);
	}

	void setStep(float f) {
		if (!physicsHandler) return;
		physicsHandler->setStepFactor(f);
	}

	void setGravity(const Vector3D& g) {
		if (physicsHandler)
			physicsHandler->setGravity(g);
	}

	void setPaused(bool p) {
		if (physicsHandler)
			physicsHandler->pause(p);
	}

	bool getPaused() {
		return physicsHandler ? physicsHandler->isPaused() : false;
	}

	void drawProperties(bool d) {
		if (!physicsHandler) return;
		physicsHandler->setDrawProperties(d);
	}

	void setIgnoreSameID(bool v) {
		if (!physicsHandler) return;
		physicsHandler->setIgnoreSameID(v);
	}
}

void bindPhysics() {
	sol::table physics = lua->create_named_table("Physics");

	physics["SetDebugMode"] = &Bind::setDebugMode;
	physics["SetStepFactor"] = &Bind::setStep;
	physics["SetGravity"] = &Bind::setGravity;
	physics["SetPaused"] = &Bind::setPaused;
	physics["IsPaused"] = &Bind::getPaused;
	physics["SetDrawProperties"] = &Bind::drawProperties;
	physics["SetIgnoreSameID"] = &Bind::setIgnoreSameID;
}