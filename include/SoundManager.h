#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

#include "Vector2D.h"
#include "Vector3D.h"

#include "Compatible3D.h"

#pragma comment(lib, "irrKlang.lib")

class SoundManager {
private:
	irrklang::ISoundEngine* soundEngine = nullptr;
	irr::scene::ISceneNode* listenerSrc = nullptr;
	irr::core::vector3df listenerPos;
public:
	SoundManager();
	~SoundManager();

	void update(); // Update listener position etc.

	void setListenerAt(const Compatible3D& node); // Allow setListenAt(nil) to clear?
	void setListenerAt(const Vector3D& pos);
	Vector3D getListenerPosition(); // Returns position of node that is listenerSrc or listenerPos if listenerSrc is nullptr

	void preloadSound(std::string path); // Can use :toStr() to get sound path
	void unloadSound(std::string path); // removeSoundSource
	void unloadAllSounds(); // removeAllSoundSources

	int getLoadedSoundsCount(); // getSoundSourceCount

	void setAllSoundsPaused(bool v); // Pauses/unpauses all sounds, soundEngine->setAllSoundsPaused
	void stopAllSounds(); // stopAllSounds
	void setDefaultVolumeRange(const Vector2D& minMax); // setDefaultMinDistance etc.
};