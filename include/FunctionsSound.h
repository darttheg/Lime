#include "FunctionsImports.h"

namespace Bind {
	int play2DSound(const std::string& filePath, bool loop = false) {
		return soundManager->play2DSound(filePath, loop);
	}

	int play3DSound(const std::string& filePath, Vector3D src, bool loop = false) {
		return soundManager->play3DSound(filePath, src, loop);
	}

	void play2DSoundOnChannel(int channel, const std::string& filePath, bool loop = false) {
		soundManager->play2DSoundOnChannel(channel, filePath, loop);
	}

	void play3DSoundOnChannel(int channel, const std::string& filePath, Vector3D src, bool loop = false) {
		soundManager->play3DSoundOnChannel(channel, filePath, src, loop);
	}

	void stopChannel(int channel) {
		soundManager->stopChannel(channel);
	}

	void pauseChannel(int channel, bool paused) {
		soundManager->pauseChannel(channel, paused);
	}

	void loopChannel(int channel, bool loop) {
		soundManager->loopChannel(channel, loop);
	}

	void stopAllSounds() {
		soundManager->stopAllSounds();
	}

	bool preloadSound(std::string path) {
		return soundManager->preloadSound(path);
	}

	void setListenerPosition(Vector3D pos, Vector3D forward) {
		soundManager->setListenerPosition(pos, forward);
	}

	bool isChannelFree(int channel) {
		return soundManager->isChannelFree(channel);
	}

	int getNextAvailableChannel() {
		return soundManager->getNextAvailableChannel();
	}

	void resetChannelFX(int channel) {
		soundManager->resetChannelFX(channel);
	}

	void setPitch(int channel, float pitch) {
		soundManager->setPitch(channel, pitch);
	}

	void setVolume(int channel, float volume) {
		soundManager->setVolume(channel, volume);
	}

	void setPan(int channel, float pan) {
		soundManager->setPan(channel, pan);
	}

	void setDistortionEffect(int channel, int effect, bool enable, sol::table params) {
		soundManager->setDistortionEffect(channel, effect, enable, params);
	}

	int validChannel(int i) {
		return soundManager->validChannel(i);
	}

	void setPlaybackSpeed(int channel, float spd) {
		soundManager->setPlaybackSpeed(channel, spd);
	}

	void setPlayPosition(int channel, int ms) {
		soundManager->setPlayPosition(channel, ms);
	}

	std::string printChannelList() {
		return soundManager->printChannelList();
	}

	void setDopplerParameters(float dop, float dist) {
		soundManager->setDopplerParameters(dop, dist);
	}

	void setChannelVelocity(int channel, float vel) {
		soundManager->SetChannelVelocity(channel, vel);
	}

	void setChannelPosition3D(int channel, const Vector3D& pos) {
		soundManager->setChannelPosition3D(channel, pos);
	}

	void setChannelDistance(int channel, float distance) {
		soundManager->setMinDistance(channel, distance);
	}
}

void bindSound() {
	sol::table sound = lua->create_named_table("Sound");

	sound["PlaySound2D"] = &Bind::play2DSound;
	sound["PlaySound3D"] = &Bind::play3DSound;
	sound["PlaySound2DOnChannel"] = &Bind::play2DSoundOnChannel;
	sound["PlaySound3DOnChannel"] = &Bind::play3DSoundOnChannel;
	sound["StopChannel"] = &Bind::stopChannel;
	sound["SetChannelPaused"] = &Bind::pauseChannel;
	sound["SetChannelLooped"] = &Bind::loopChannel;
	sound["StopAllChannels"] = &Bind::stopAllSounds;
	sound["PreloadSound"] = &Bind::preloadSound;
	sound["SetListenerPosition"] = &Bind::setListenerPosition;
	sound["ClearChannelEffects"] = &Bind::resetChannelFX;
	sound["SetChannelEffect"] = &Bind::setDistortionEffect;
	sound["SetChannelVolume"] = &Bind::setVolume;
	sound["SetChannelPitch"] = &Bind::setPitch;
	sound["SetChannelPan"] = &Bind::setPan;
	sound["GetChannelList"] = &Bind::printChannelList;
	sound["SetChannelPlaybackSpeed"] = &Bind::setPlaybackSpeed;
	sound["SetChannelPlaybackPosition"] = &Bind::setPlayPosition;
	sound["SetChannelVelocity"] = &Bind::setChannelVelocity;
	sound["SetDopplerEffectParameters"] = &Bind::setDopplerParameters;
	sound["SetChannelPosition3D"] = &Bind::setChannelPosition3D;
	sound["IsChannelEmpty"] = &Bind::isChannelFree;
	sound["SetChannelMinimumDistance"] = &Bind::setChannelDistance;
}