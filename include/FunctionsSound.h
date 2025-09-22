#include "FunctionsImports.h"

namespace Bind {
	int getVolume() {
		return soundManager->getMainVolume();
	}

	void setVolume(int f) {
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

	void setAllSoundsPaused(bool v) {
		soundManager->setAllSoundsPaused(v);
	}

	void stopAll() {
		soundManager->stopAllSounds();
	}

	void setDefaultDistanceRange(const Vector2D& minMax) {
		soundManager->setDefaultVolumeRange(minMax);
	}

	void setDopplerParameters(float d, float di) {
		soundManager->setDopplerEffectParameters(d, di);
	}
}

void bindAudio() {
	sol::table audio = lua->create_named_table("Audio");

	audio["GetVolume"] = Bind::getVolume;
	audio["SetVolume"] = Bind::setVolume;
	audio["SetListenerVelocity"] = Bind::setListenerVelocity;
	audio["SetManualListener"] = Bind::setManualListener;
	audio["Unload"] = &Bind::unloadSound;
	audio["UnloadAll"] = &Bind::unloadAllSounds;
	audio["GetLoadedSoundsCount"] = &Bind::getLoadedSoundsCount;
	audio["SetAllPaused"] = &Bind::setAllSoundsPaused;
	audio["StopAll"] = &Bind::stopAll;
	audio["SetDefaultVolumeDistanceRange"] = &Bind::setDefaultDistanceRange;
	audio["SetDopplerParameters"] = &Bind::setDopplerParameters;
}