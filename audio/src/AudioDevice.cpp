#include "AudioDevice.hpp"
#include <exception>
#include <format>

audio::AudioDevice::AudioDevice()
{
	audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (audio_device == 0)
	{
	    throw std::runtime_error{ std::format("Couldn't open audio device: {}", SDL_GetError()) };
	}
}

audio::AudioDevice::~AudioDevice()
{
	SDL_CloseAudioDevice(audio_device);
}

SDL_AudioDeviceID audio::AudioDevice::getAudioDeviceID() const
{
	return audio_device;
}
