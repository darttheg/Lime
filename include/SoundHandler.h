#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>

#include "Vector2D.h"
#include "Vector3D.h"

#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;
using namespace irr;
using namespace core;

#include <vector>

struct SoundAttachedToPos {
	irrklang::ISound* src = nullptr;
	irr::scene::ISceneNode* posSrc = nullptr;

	SoundAttachedToPos(irrklang::ISound* s, irr::scene::ISceneNode* p) : src(s), posSrc(p) {};
};

class SoundManager {
private:
	irrklang::ISoundEngine* soundEngine = nullptr;

	irr::core::vector3df nonCamPos, nonCamForward, listenerVel, nonCamUp;
	std::vector<SoundAttachedToPos> attachedToObjs; // Push sounds to this array if attached to a node, erase if object is nil or removed manually.

	bool manualListenThisFrame = false;
public:
	SoundManager();
	~SoundManager();

	irrklang::ISoundEngine* getEngine();

	void update(); // Update listener position etc.

	float getMainVolume();
	void setMainVolume(float f);

	void setListenerVelocity(const Vector3D& vel);
	void setManualListener(const Vector3D& pos, const Vector3D& forward, const Vector3D& velocity, const Vector3D& up);

	ISoundSource* preloadSound(std::string path); // Can use :toStr() to get sound path
	void unloadSound(std::string path); // removeSoundSource
	void unloadAllSounds(); // removeAllSoundSources

	int getLoadedSoundsCount(); // getSoundSourceCount

	void setAllSoundsPaused(bool v); // Pauses/unpauses all sounds, soundEngine->setAllSoundsPaused
	void stopAllSounds(); // stopAllSounds
	void setDefaultVolumeRange(const Vector2D& minMax); // setDefaultMinDistance etc.
	void setDopplerEffectParameters(float dopplerFactor, float distanceFactor);

	void pushSoundPosEntry(irrklang::ISound* key, irr::scene::ISceneNode * srcPos);
	void removeSoundPosEntry(irrklang::ISound* key);
};