#include "SoundManager.h"
using namespace irrklang;

SoundManager::SoundManager() {
	soundEngine = irrklang::createIrrKlangDevice();

	nonCamForward = vector3df(0, 0, 1);
	nonCamUp = vector3df(0, 1, 0);
}

void SoundManager::update() {
	if (listenerSrc && listenerSrc->getNode()) {
		vector3df pos, forward, up;

		pos = listenerSrc->getNode()->getAbsolutePosition();
		Vector3D f = listenerSrc->getForward();
		forward = vector3df(f.x, f.y, f.z);
		Vector3D u = listenerSrc->getUp();
		up = vector3df(u.x, u.y, u.z);

		soundEngine->setListenerPosition(pos, forward, listenerVel, up);
	}
	else {
		soundEngine->setListenerPosition(nonCamPos, nonCamForward, listenerVel, nonCamUp);
	}

	// Set attached node positions, check if sound is finished and remove it if so.
}

void SoundManager::makeCameraListener(Camera3D* cam) {
	listenerSrc = cam;
}

void SoundManager::setListenerVelocity(const Vector3D& vel) {
	listenerVel = vector3df(vel.x, vel.y, vel.z);
}

void SoundManager::setManualListener(const Vector3D& pos, const Vector3D& forward, const Vector3D& velocity, const Vector3D& up) {
	listenerSrc = nullptr;

	nonCamPos = vector3df(pos.x, pos.y, pos.z);
	nonCamForward = vector3df(forward.x, forward.y, forward.z);
	listenerVel = vector3df(velocity.x, velocity.y, velocity.z);
	nonCamUp = vector3df(up.x, up.y, up.z);
}

ISoundSource* SoundManager::preloadSound(std::string path) {
	return soundEngine->addSoundSourceFromFile(path.c_str(), E_STREAM_MODE::ESM_AUTO_DETECT, true);
}

void SoundManager::unloadSound(std::string path) {
	soundEngine->removeSoundSource(path.c_str());
}

void SoundManager::unloadAllSounds() {
	soundEngine->removeAllSoundSources();
}

void SoundManager::setAllSoundsPaused(bool v) {
	soundEngine->setAllSoundsPaused(v);
}

int SoundManager::getLoadedSoundsCount() {
	return soundEngine->getSoundSourceCount();
}

void SoundManager::setDefaultVolumeRange(const Vector2D& minMax) {
	soundEngine->setDefault3DSoundMinDistance(minMax.x);
	soundEngine->setDefault3DSoundMaxDistance(minMax.y);
}

void SoundManager::setDopplerEffectParameters(float dopplerFactor, float distanceFactor) {
	soundEngine->setDopplerEffectParameters(dopplerFactor, distanceFactor);
}

irrklang::ISoundEngine* SoundManager::getEngine() {
	return soundEngine;
}
