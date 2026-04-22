#pragma once

#include <SDL3/SDL_audio.h>
#include <filesystem>
#include <vector>

namespace audio
{
	class Sound
	{
	public:
		enum class PlayType
		{
			STOP_ON_PLAY,
			DONT_PLAY_WHEN_PLAYING
		};
		
		Sound() = default;
		Sound(SDL_AudioDeviceID audio_device, const std::filesystem::path& path);
		~Sound();

		void setVolume(float percentage);
		void setPlayType(PlayType type);
		
		float getVolume() const;

		void play();
		void stop();
	private:
		Uint8* wav_data = nullptr;
		Uint32 wav_data_len = 0;
		SDL_AudioSpec spec;
		SDL_AudioStream* stream = nullptr;
		float volume = 1.0f;
		
		PlayType play_type = PlayType::STOP_ON_PLAY;
	};
}
