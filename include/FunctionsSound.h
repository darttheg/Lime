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

	void setManualListener(const Vector3D& pos = Vector3D(), const Vector3D& forward = Vector3D(), const Vector3D& velocity = Vector3D(), const Vector3D& up = Vector3D(0, 1, 0)) {
		soundManager->setManualListener(pos, forward, velocity, up);
	}

	void unloadSound(std::string path) {
		soundManager->unloadSound(path);
	}

	void unloadAllSounds() {
		soundManager->unloadAllSounds();
	}

	int getLoadedSoundsCount() {
		return soundManager->getLoadedSoundsCount();
	}
}

void bindSound() {
	sol::table audio = lua->create_named_table("Audio");

	audio["getVolume"] = Bind::getVolume;
	audio["setVolume"] = Bind::setVolume;
	audio["setListenerVelocity"] = Bind::setListenerVelocity;
	audio["setManualListener"] = Bind::setManualListener;
	audio["unload"] = &Bind::unloadSound;
	audio["unloadAll"] = &Bind::unloadAllSounds;
	audio["getLoadedSoundsCount"] = &Bind::getLoadedSoundsCount;
}