#include "Sound.h"
#include "Compatible3D.h"

// Sound::Sound() : Sound("", false, false) {}

Sound::Sound(std::string path) : Sound(path, 0, false) {}

Sound::Sound(std::string path, int playbackType) : Sound(path, playbackType, false) {}

Sound::Sound(std::string nPath, int playbackType, bool doLoop) {
    load(nPath, playbackType);
    loops = doLoop;
}

bool Sound::load(std::string nPath, int playbackType, bool doEffects) {
    path = nPath;
    soundSrc = soundManager->getEngine()->addSoundSourceFromFile(path.c_str(), playbackType == 0 ? E_STREAM_MODE::ESM_NO_STREAMING : E_STREAM_MODE::ESM_STREAMING, playbackType == 0);
    doSFX = doEffects;

    if (d && soundSrc) d->text = soundSrc->getName();

    return soundSrc != nullptr;
}

void Sound::play(bool is3D, bool startPaused) {
    if (!soundSrc) return;

    if (is3D)
        mySound = soundManager->getEngine()->play3D(soundSrc, playPos3D, loops, startPaused, true, doSFX);
    else
        mySound = soundManager->getEngine()->play2D(soundSrc, loops, startPaused, true, doSFX);

    if (!startPaused && is3D) doDebugVisual();
}

void Sound::stop() {
    if (!soundSrc || !mySound) return;
    mySound->stop();
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

int Sound::getPlayPosition() {
    return mySound ? mySound->getPlayPosition() : 0;
}

void Sound::setPlayPosition(int ms) {
    if (!mySound) return;
    mySound->setPlayPosition(ms);
}

int Sound::getPlayLength() {
    return mySound ? mySound->getPlayLength() : 0;
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

    if (!attached && d)
        d->setPosition(vector3df(playPos3D.X, playPos3D.Y, playPos3D.Z));
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
    soundManager->pushSoundPosEntry(mySound, obj.getNode(), dHolder);
}

void Sound::detach() {
    if (!attached) return;

    soundManager->removeSoundPosEntry(mySound);
    attached = false;

    if (d) d->setPosition(vector3df(playPos3D.X, playPos3D.Y, playPos3D.Z));
}

void Sound::destroy() {
    detach();
    if (mySound) mySound->drop();
    if (soundSrc) soundSrc->drop();
    if (d) d->drop();
    if (dHolder) dHolder->drop();
    attached = false;
}

std::string Sound::toStr() {
    return mySound ? path : "";
}

bool Sound::getDebug() {
    return doDebug;
}

void Sound::setDebug(bool v) {
    doDebug = v;
    doDebugVisual();
}

void Sound::doDebugVisual() {
    if (!d && doDebug && mySound) {
        dHolder = smgr->addEmptySceneNode();
        d = new DebugSceneNode(dHolder, smgr, 0, DebugType::SOUND);
        vector3df pos = vector3df(playPos3D.X, playPos3D.Y, playPos3D.Z);
        d->setPosition(pos);
        if (soundSrc) d->text = soundSrc->getName();
    }
    else if (!doDebug && d) {
        d->drop();
        dHolder->drop();
    }
}

void bindSound() {
    sol::usertype<Sound> bindType = lua->new_usertype<Sound>("Sound",
        sol::constructors<Sound(std::string path), Sound(std::string path, int playbackType), Sound(std::string path, int playbackType, bool loops)>(),
        
        "debug", sol::property(&Sound::getDebug, &Sound::setDebug),
        "paused", sol::property(&Sound::isPaused, &Sound::setPaused),
        "loops", sol::property(&Sound::getLoops, &Sound::setLoops),
        "volume", sol::property(&Sound::getVolume, &Sound::setVolume),
        "playbackSpeed", sol::property(&Sound::getPitch, &Sound::setPitch),
        "pan", sol::property(&Sound::getPan, &Sound::setPan),
        "paused", sol::property(&Sound::isPaused, &Sound::setPaused),

        "velocity", sol::property(
            [](Sound& c) { return Vector3D{ [&] { return c.getVelocity(); }, [&](auto v) { c.setVelocity(v); } }; },
            [](Sound& c, const Vector3D& v) { c.setVelocity(v); }
        ),

        "position", sol::property(
            [](Sound& c) { return Vector3D{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](Sound& c, const Vector3D& v) { c.setPosition(v); }
        ));

    bindType["play"] = &Sound::play;
    bindType["load"] = &Sound::load;
    bindType["stop"] = &Sound::stop;
    bindType["isPlaying"] = &Sound::isPlaying;
    bindType["getPlayLength"] = &Sound::getPlayLength;

    bindType["clearEffects"] = &Sound::clearEffects;
    bindType["addDistortionEffect"] = &Sound::addDistortionEffect;
    bindType["addEchoEffect"] = &Sound::addEchoEffect;
    bindType["addReverbEffect"] = &Sound::addReverbEffect;
    bindType["setVolumeDistanceRange"] = &Sound::setVolumeDistanceRange;
    bindType["setStreamMode"] = &Sound::setSourceStreamMode;

    bindType["attach"] = &Sound::attachToObject;
    bindType["detach"] = &Sound::detach;

    bindType["destroy"] = &Sound::destroy;
    bindType["toStr"] = &Sound::toStr;
}