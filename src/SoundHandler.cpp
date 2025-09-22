#include "SoundHandler.h"
#include "IrrManagers.h"
#include <algorithm>

SoundManager::SoundManager() {
	soundEngine = irrklang::createIrrKlangDevice();

	nonCamForward = vector3df(0, 0, 1);
	nonCamUp = vector3df(0, 1, 0);
}

SoundManager::~SoundManager() {
}

void SoundManager::update() {
	if (!manualListenThisFrame) {
		vector3df pos, forward, up;

		// Use active camera

		pos = smgr->getActiveCamera()->getAbsolutePosition();
		forward = mainCameraForward->getAbsolutePosition() - pos;
		forward.normalize();
		up = smgr->getActiveCamera()->getUpVector();

		soundEngine->setListenerPosition(pos, forward, listenerVel, up);
	}
	else {
		soundEngine->setListenerPosition(nonCamPos, nonCamForward, listenerVel, nonCamUp);
	}

	// Set attached node positions, check if sound is finished and remove it if so.
	attachedToObjs.erase(
		std::remove_if(attachedToObjs.begin(), attachedToObjs.end(),
			[](const SoundAttachedToPos& e) { return !e.posSrc || !e.src; }),
		attachedToObjs.end()
	);

	for (auto& entry : attachedToObjs)
		entry.src->setPosition(entry.posSrc->getAbsolutePosition());
}

int SoundManager::getMainVolume() {
	return soundEngine ? soundEngine->getSoundVolume() * 100 : 0;
}

void SoundManager::setMainVolume(int f) {
	if (!soundEngine) return;
	soundEngine->setSoundVolume(f / 100.0);
}

void SoundManager::setListenerVelocity(const Vector3D& vel) {
	listenerVel = vector3df(vel.x, vel.y, vel.z);
}

void SoundManager::setManualListener(const Vector3D& pos, const Vector3D& forward, const Vector3D& velocity, const Vector3D& up) {
	manualListenThisFrame = true;

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

void SoundManager::stopAllSounds() {
	soundEngine->stopAllSounds();
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

void SoundManager::pushSoundPosEntry(ISound* key, ISceneNode* srcPos) {
	auto it = std::find_if(attachedToObjs.begin(), attachedToObjs.end(),
		[key](const SoundAttachedToPos& entry) {
			return entry.src == key;
		});

	if (it == attachedToObjs.end())
		attachedToObjs.emplace_back(key, srcPos);
	else
		it->posSrc = srcPos;
}

void SoundManager::removeSoundPosEntry(ISound* key) {
	attachedToObjs.erase(
		std::remove_if(attachedToObjs.begin(), attachedToObjs.end(),
			[key](const SoundAttachedToPos& entry) {
				return entry.src == key;
			}),
		attachedToObjs.end()
	);
}

irrklang::ISoundEngine* SoundManager::getEngine() {
	return soundEngine;
}
