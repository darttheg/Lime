#include "Sound.h"

// Sound::Sound() : Sound("", false, false) {}

Sound::Sound(std::string path, int playbackType = 0) : Sound(path, playbackType, false) {}

Sound::Sound(std::string nPath, int playbackType = 0, bool doLoop = false) {
    load(nPath, playbackType);
    loops = doLoop;
}

bool Sound::load(std::string nPath, int playbackType, bool doEffects) {
    path = nPath;
    if (playbackType == 0) { // Static
        soundSrc = soundManager->preloadSound(path);
    }
    doSFX = doEffects;

    return soundSrc != nullptr;
}

void Sound::play(bool is3D, bool startPaused = false) {
    if (!soundSrc) return;

    if (is3D)
        mySound = soundManager->getEngine()->play3D(soundSrc, playPos3D, loops, startPaused, true, doSFX);
    else
        mySound = soundManager->getEngine()->play2D(soundSrc, loops, startPaused, true, doSFX);
}

void Sound::setPaused(bool v) {
    if (!mySound) return;
    mySound->setIsPaused(v);
}

bool Sound::isPaused() {
    return mySound ? mySound->getIsPaused() : false;
}

bool Sound::isPlaying() {
    return mySound ? !mySound->isFinished() : false;
}

void Sound::setLoops(bool v) {
    loops = v;
    if (mySound)
        mySound->setIsLooped(loops);
}

bool Sound::getLoops() {
    return mySound ? loops : false;
}

void Sound::setVolume(float v) {
    if (mySound)
        mySound->setVolume(v);
}

float Sound::getVolume() {
    return mySound ? mySound->getVolume() : 0.0f;
}

void Sound::setPitch(float v) {
    if (mySound)
        mySound->setPlaybackSpeed(v);
}

float Sound::getPitch() {
    return mySound ? mySound->getPlaybackSpeed() : 0.0f;
}

void Sound::setPan(float v) {
    if (mySound)
        mySound->setPan(v);
}

float Sound::getPan() {
    return mySound ? mySound->getPan() : 0.0f;
}

void Sound::setVelocity(const Vector3D& vel) {
    if (mySound)
        mySound->setVelocity(vec3df(vel.x, vel.y, vel.z));
}

Vector3D Sound::getVelocity() {
    return mySound ? Vector3D(mySound->getVelocity().X, mySound->getVelocity().Y, mySound->getVelocity().Z) : Vector3D();
}

void Sound::setPosition(const Vector3D& pos) {
    playPos3D = vec3df(pos.x, pos.y, pos.z);
}

Vector3D Sound::getPosition() {
    return Vector3D(playPos3D.X, playPos3D.Y, playPos3D.Z);
}

void Sound::clearEffects() {
    if (!mySound) return;
    mySound->getSoundEffectControl()->disableAllEffects();
}

void Sound::addDistortionEffect(float gain, float edge) {
    if (!mySound) return;
    mySound->getSoundEffectControl()->enableDistortionSoundEffect(gain, edge);
}

void Sound::addEchoEffect(float wetDry, float feedback, float delay) {
    if (!mySound) return;
    mySound->getSoundEffectControl()->enableEchoSoundEffect(wetDry, feedback, delay, delay);
}

void Sound::addReverbEffect(float inputGain, float mix, float time, float freqRatio) {
    if (!mySound) return;
    mySound->getSoundEffectControl()->enableWavesReverbSoundEffect(inputGain, mix, time, freqRatio);
}

void Sound::setVolumeDistanceRange(const Vector2D& minMax) {
    if (!mySound) return;
    mySound->setMinDistance(minMax.x);
    mySound->setMaxDistance(minMax.y);
}

void Sound::setSourceStreamMode(int mode) {
    if (!soundSrc) return;
    
    soundSrc->setStreamMode(mode == 0 ? E_STREAM_MODE::ESM_NO_STREAMING : E_STREAM_MODE::ESM_STREAMING);
}

void Sound::attachToObject(const Compatible3D& obj) {
    soundManager->pushSoundPosEntry(mySound, obj.getNode());
}

void Sound::attachToObject() {
    detach();
}

void Sound::detach() {
    if (!attached) return;

    soundManager->removeSoundPosEntry(mySound);
    attached = false;
}

void Sound::destroy() {
    if (mySound) mySound->drop();
    if (soundSrc) soundSrc->drop();
}

std::string Sound::toStr() {
    return mySound ? path : "";
}
