#include "Sound.h"
#include "IrrManagers.h"

SoundManager::SoundManager()
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
    {
        channels[i] = channel();
    }
}

SoundManager::~SoundManager()
{
    //stopAllSounds();
}

int SoundManager::play2DSound(const std::string& filePath, bool loop)
{
    int channel = getNextAvailableChannel();
    play2DSoundOnChannel(channel, filePath, loop);
    return channel;
}

int SoundManager::play3DSound(const std::string& filePath, Vector3D src, bool loop)
{
    int channel = getNextAvailableChannel();
    play3DSoundOnChannel(channel, filePath, src, loop);
    return channel;
}

void SoundManager::play2DSoundOnChannel(int channel, const std::string& filePath, bool loop)
{
    channel = validChannel(channel);
    stopChannel(channel);
    channels[channel].sound = sound->play2D(filePath.c_str(), loop, true, true, ESM_AUTO_DETECT, true);

    if (!channels[channel].sound) return;

    channels[channel].sound->setVolume(channels[channel].volume);
    channels[channel].sound->setPan(channels[channel].pan);
    channels[channel].sound->setPlaybackSpeed(channels[channel].spd);
    channels[channel].sound->setMinDistance(channels[channel].minDist);
    channels[channel].sound->setIsPaused(false);
}

void SoundManager::play3DSoundOnChannel(int channel, const std::string& filePath, Vector3D src, bool loop)
{
    channel = validChannel(channel);
    stopChannel(channel);

    channels[channel].sound = sound->play3D(filePath.c_str(), vec3df(static_cast<f32>(src.x), static_cast<f32>(src.y), static_cast<f32>(src.z)), loop, true, true, ESM_AUTO_DETECT, true);

    if (!channels[channel].sound) return;

    channels[channel].sound->setVolume(channels[channel].volume);
    channels[channel].sound->setPan(channels[channel].pan);
    channels[channel].sound->setPlaybackSpeed(channels[channel].spd);
    channels[channel].sound->setMinDistance(channels[channel].minDist);
    channels[channel].sound->setIsPaused(false);
}

void SoundManager::stopChannel(int channel)
{
    channel = validChannel(channel);
    if (channels[channel].sound)
    {
        channels[channel].sound->stop();
        channels[channel].sound = nullptr;
    }
}

void SoundManager::pauseChannel(int channel, bool paused)
{
    channel = validChannel(channel);
    if (channels[channel].sound)
    {
        channels[channel].sound->setIsPaused(paused);
    }
}

void SoundManager::loopChannel(int channel, bool loop)
{
    channel = validChannel(channel);
    if (channels[channel].sound)
    {
        channels[channel].sound->setIsLooped(loop);
    }
}

void SoundManager::stopAllSounds()
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
    {
        stopChannel(i);
    }
}

bool SoundManager::preloadSound(std::string path)
{
    ISoundSource* s = sound->addSoundSourceFromFile(path.c_str());
    return s != nullptr;
}

void SoundManager::setDopplerParameters(float dopFactor, float distFactor) {
    sound->setDopplerEffectParameters(dopFactor, distFactor);
}

void SoundManager::SetChannelVelocity(int channel, const Vector3D& velocity) {
    channel = validChannel(channel);
    if (channels[channel].sound) {
        channels[channel].sound->setVelocity(vec3df(static_cast<f32>(velocity.x), static_cast<f32>(velocity.y), static_cast<f32>(velocity.z)));
    }
}

void SoundManager::setChannelPosition3D(int channel, const Vector3D& pos) {
    channel = validChannel(channel);
    if (channels[channel].sound) {
        channels[channel].sound->setPosition(vec3df(static_cast<f32>(pos.x), static_cast<f32>(pos.y), static_cast<f32>(pos.z)));
    }
}

void SoundManager::setListenerPosition(const Vector3D& pos, const Vector3D& forward)
{
    sound->setListenerPosition(vec3df(static_cast<f32>(pos.x), static_cast<f32>(pos.y), static_cast<f32>(pos.z)),
        vec3df(static_cast<f32>(forward.x), static_cast<f32>(forward.y), static_cast<f32>(forward.z)));
}

bool SoundManager::isChannelFree(int channel)
{
    channel = validChannel(channel);
    if (!channels[channel].sound)
        return true;
    if (channels[channel].sound->isFinished())
        return true;
    return false;
}

int SoundManager::getNextAvailableChannel()
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
    {
        if (isChannelFree(i))
        {
            return i;
        }
    }
    return MAX_CHANNELS - 1; // Default to last channel
}

void SoundManager::resetChannelFX(int channel)
{
    channel = validChannel(channel);
    if (channels[channel].sound)
    {
        channels[channel].spd = 1.0f;
        channels[channel].volume = 1.0f;
        channels[channel].pan = 0.0f;
        channels[channel].minDist = 1.0f;

        channels[channel].sound->setPlaybackSpeed(1.0f);
        channels[channel].sound->setVolume(1.0f);
        channels[channel].sound->setPan(0.0f);
        channels[channel].sound->setMinDistance(1.0f);
    }
}

void SoundManager::setPitch(int channel, float pitch)
{
    // Ignore
    channel = validChannel(channel);
    channels[channel].spd = pitch;
    if (channels[channel].sound)
    {
        channels[channel].sound->setPlaybackSpeed(pitch);
    }
}

void SoundManager::setVolume(int channel, float volume)
{
    channel = validChannel(channel);
    channels[channel].volume = volume;
    if (channels[channel].sound)
    {
        channels[channel].sound->setVolume(volume);
    }
}

void SoundManager::setPlaybackSpeed(int channel, float spd)
{
    channel = validChannel(channel);
    channels[channel].spd = spd;
    if (channels[channel].sound)
    {
        channels[channel].sound->setPlaybackSpeed(spd);
    }
}

void SoundManager::setPlayPosition(int channel, int time)
{
    channel = validChannel(channel);
    if (channels[channel].sound)
    {
        channels[channel].sound->setPlayPosition(time);
    }
}

void SoundManager::setPan(int channel, float pan)
{
    channel = validChannel(channel);
    channels[channel].pan = pan;
    if (channels[channel].sound)
    {
        channels[channel].sound->setPan(pan);
    }
}

void SoundManager::setMinDistance(int channel, float dist)
{
    channel = validChannel(channel);
    channels[channel].minDist = dist;
    if (channels[channel].sound)
    {
        channels[channel].sound->setMinDistance(dist);
    }
}

void SoundManager::setDistortionEffect(int channel, int effect, bool enable, sol::table params)
{
    ISoundEffectControl* fx = 0;
    channel = validChannel(channel);
    if (channels[channel].sound)
    {
        fx = channels[channel].sound->getSoundEffectControl();
        if (!fx)
        {
            return;
        }
        if (effect < 0) effect = 0;
        if (effect > 4) effect = 4;

        switch (effect)
        {
        case 0:
            if (enable)
            {
                u32 gain = -18.0f;
                u32 edge = 15.0f;
                if (params["gain"].valid()) gain = params["gain"];
                if (params["edge"].valid()) edge = params["edge"];
                fx->enableDistortionSoundEffect(gain, edge);
            }
            else
            {
                fx->disableDistortionSoundEffect();
            }
            break;

        case 1:
            if (enable)
            {
                u32 wetDry = 50.0f;
                u32 feedback = 50.0f;
                u32 delay = 500.0f;
                if (params["wetDry"].valid()) wetDry = params["wetDry"];
                if (params["feedback"].valid()) feedback = params["feedback"];
                if (params["delay"].valid()) delay = params["delay"];
                fx->enableEchoSoundEffect(wetDry, feedback, delay, delay, 0);
            }
            else
            {
                fx->disableEchoSoundEffect();
            }
            break;

        case 2:
            if (enable)
            {
                u32 inputGain = 0.0f;
                u32 reverbMix = 0.0f;
                u32 reverbTime = 1000.0f;
                u32 frequencyRatio = 0.001;
                if (params["inputGain"].valid()) inputGain = params["inputGain"];
                if (params["reverbMix"].valid()) reverbMix = params["reverbMix"];
                if (params["reverbTime"].valid()) reverbTime = params["reverbTime"];
                if (params["frequencyRatio"].valid()) frequencyRatio = params["frequencyRatio"];
                fx->enableWavesReverbSoundEffect(inputGain, reverbMix, reverbTime, frequencyRatio);
            }
            else
            {
                fx->disableWavesReverbSoundEffect();
            }
            break;

        case 3:
            if (enable)
            {
                f32 fCenter = 8000.0f;
                f32 fBandwidth = 12.0f;
                f32 fGain = 0.0f;
                if (params["frequency"].valid()) fCenter = params["frequency"];
                if (params["bandwidth"].valid()) fBandwidth = params["bandwidth"];
                if (params["gain"].valid()) fGain = params["gain"];
                fx->enableParamEqSoundEffect(fCenter, fBandwidth, fGain);
            }
            else
            {
                fx->disableParamEqSoundEffect();
            }
            break;

        case 4:
            if (enable)
            {
                ik_s32 rateHz = 20;
                bool sinusWaveForm = true;
                if (params["rate"].valid()) rateHz = params["rate"];
                if (params["sinusWaveForm"].valid()) sinusWaveForm = params["sinusWaveForm"];
                fx->enableGargleSoundEffect(rateHz, sinusWaveForm);
            }
            else
            {
                fx->disableGargleSoundEffect();
            }
            break;

        }
    }
}

int SoundManager::validChannel(int i)
{
    if (i < 0) return 0;
    if (i >= MAX_CHANNELS) return MAX_CHANNELS - 1;
    return i;
}

std::string SoundManager::printChannelList()
{
    std::string result;
    for (int i = 0; i < MAX_CHANNELS; ++i)
    {
        std::string status = std::to_string(i) + ": ";
        if (channels[i].sound && !channels[i].sound->isFinished())
        {
            status += channels[i].sound->getSoundSource()->getName();
        }
        result += status + "\n"; // Append to the result string with a newline
    }
    return result;
}
