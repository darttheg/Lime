#pragma once

#include "IrrManagers.h"
#pragma comment(lib, "irrKlang.lib")

class Compatible3D;

class Sound {
private:
	irrklang::ISound* mySound = nullptr;
	irrklang::ISoundSource* soundSrc = nullptr;
	bool loops = false;
	bool doSFX = false;
	vec3df playPos3D;
	std::string path = "";

	bool attached = false;

public:
	// Sound();
	Sound(std::string path, int playbackType);
	Sound(std::string path, int playbackType, bool loops);
	Sound(std::string path);

	bool load(std::string path, int playbackType = 0, bool doEffects = false); // Starts paused, just loads the sound to be ready for play
	void play(bool is3D = false, bool startPaused = false);

	void setPaused(bool v);
	bool isPaused();
	bool isPlaying();

	void setLoops(bool v);
	bool getLoops();

	void setVolume(float f);
	float getVolume();

	void setPitch(float f);
	float getPitch();

	void setPan(float f);
	float getPan();

	int getPlayPosition();
	void setPlayPosition(int ms);

	int getPlayLength();

	void setVelocity(const Vector3D& vel);
	Vector3D getVelocity();

	void setPosition(const Vector3D& pos);
	Vector3D getPosition();

	void clearEffects();
	void addDistortionEffect(float gain, float edge);
	void addEchoEffect(float wetDry, float feedback, float delay);
	void addReverbEffect(float inputGain, float mix, float time, float freqRatio);

	void setVolumeDistanceRange(const Vector2D& minMax);

	void setSourceStreamMode(int mode = 0);

	void attachToObject(const Compatible3D& obj); // Update sound position per frame, push to SoundManager vector
	void detach();

	void destroy();
	std::string toStr(); // Returns path
};

void bindSound();