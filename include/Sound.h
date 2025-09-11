#pragma once

#include "SoundManager.h"

class Sound {
private:
	irrklang::ISound* mySound = nullptr;
	vector3df playPos3D;

	irr::scene::ISceneNode* src = nullptr;
public:
	Sound();
	Sound(std::string path, bool startPaused = false);

	void load(std::string path);
	void play(bool is3D = false);

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

	void setVelocity(const Vector3D& vel);
	Vector3D getVelocity();

	void setDopplerParameters(float dopplerFactor, float distanceFactor);

	void setPosition(const Vector3D& pos);
	Vector3D getPosition();

	void clearEffects();
	void addDistortionEffect(float gain, float edge);
	void addEchoEffect(float wetDry, float feedback, float delay);
	void addReverbEffect(float inputGain, float mix, float time, float freqRatio);

	void setPlaybackSpeed(float f);
	float getPlaybackSpeed();

	void setVolumeDistanceRange(const Vector2D& minMax);

	void setStreamingMode(int type);

	void attachToObject(const Compatible3D& obj); // Update sound position per frame
	void attachToObject(); // Detach

	void destroy();
	std::string toStr(); // Returns path
};

void bindSound();