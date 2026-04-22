#pragma once

#include <SDL3/SDL_audio.h>
#include <filesystem>
#include <vector>

namespace audio
{
	class Sound
	{
		enum class PlayType
		{
			STOP_ON_PLAY,
			DONT_PLAY_WHEN_PLAYING
		};
	public:
		Sound() = default;
		Sound(SDL_AudioDeviceID audio_device, const std::filesystem::path& path);
		~Sound();

		void setVolume(float percentage);

		void play();
		void stop();
	private:
		Uint8* wav_data = nullptr;
		Uint32 wav_data_len = 0;
		SDL_AudioSpec spec;
		SDL_AudioStream* stream = nullptr;
		
		PlayType play_type = PlayType::DONT_PLAY_WHEN_PLAYING;
	};
}
