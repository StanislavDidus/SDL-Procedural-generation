#pragma once
#include "SDL3/SDL_audio.h"

namespace audio
{
	class AudioDevice
	{
	public:
		AudioDevice();
		~AudioDevice();

		SDL_AudioDeviceID getAudioDeviceID() const;
	private:
		SDL_AudioDeviceID audio_device = 0;
	};
}
