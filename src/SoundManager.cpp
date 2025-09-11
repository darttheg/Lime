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

bool SoundManager::preloadSound(std::string path) {
	ISoundSource* s = soundEngine->addSoundSourceFromFile(path.c_str());
	return s != nullptr;

	// Allow for different streaming modes
}
