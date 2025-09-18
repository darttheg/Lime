#include "FunctionsImports.h"

namespace Bind {
	float getVolume() {
		return soundManager->getMainVolume();
	}

	void setVolume(float f) {
		soundManager->setMainVolume(f);
	}

	void setListenerVelocity(const Vector3D& vel) {
		soundManager->setListenerVelocity(vel);
	}
}

void bindSound() {
	sol::table audio = lua->create_named_table("Audio");

	audio["getVolume"] = Bind::getVolume;
	audio["setVolume"] = Bind::setVolume;
	audio["setListenerVelocity"] = Bind::setListenerVelocity;

}