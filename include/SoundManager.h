#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

#include "Vector2D.h"
#include "Vector3D.h"
#include "Camera3D.h"

#include "Compatible3D.h"

#pragma comment(lib, "irrKlang.lib")

class SoundManager {
private:
	irrklang::ISoundEngine* soundEngine = nullptr;
	Camera3D* listenerSrc;

	irr::core::vector3df nonCamPos, nonCamForward, listenerVel, nonCamUp;

public:
	SoundManager();
	~SoundManager();

	void update(); // Update listener position etc.

	void makeCameraListener(Camera3D* cam);
	void setListenerVelocity(const Vector3D& vel);
	void setManualListener(const Vector3D& pos = Vector3D(), const Vector3D& forward = Vector3D(), const Vector3D& velocity = Vector3D(), const Vector3D& up = Vector3D(0,1,0));

	bool preloadSound(std::string path); // Can use :toStr() to get sound path
	void unloadSound(std::string path); // removeSoundSource
	void unloadAllSounds(); // removeAllSoundSources

	int getLoadedSoundsCount(); // getSoundSourceCount

	void setAllSoundsPaused(bool v); // Pauses/unpauses all sounds, soundEngine->setAllSoundsPaused
	void stopAllSounds(); // stopAllSounds
	void setDefaultVolumeRange(const Vector2D& minMax); // setDefaultMinDistance etc.
};