#pragma once

#include <SDL3/SDL_audio.h>
#include <filesystem>

namespace audio
{
	class Sound
	{
	public:
		Sound() = default;
		explicit Sound(SDL_AudioDeviceID audio_device, const std::filesystem::path& path);
		~Sound();

		void setVolume(float percentage);

		void play();
		void update();
	private:
		Uint8* wav_data = nullptr;
		Uint32 wav_data_len = 0;
		SDL_AudioSpec spec;
		SDL_AudioDeviceID audio_device;
		std::vector<SDL_AudioStream*> streams;
		float volume = 1.0f;
	};
}
