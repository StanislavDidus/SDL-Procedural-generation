#include "Sound.hpp"
#include <exception>
#include <format>

#include "SDL3/SDL_filesystem.h"

namespace audio
{
	Sound::Sound(SDL_AudioDeviceID audio_device, const std::filesystem::path& path)
	{
		if (!SDL_LoadWAV(path.string().c_str(), &spec, &wav_data, &wav_data_len))
		{
			throw std::runtime_error{ std::format("Could not load .wav file: {}", SDL_GetError()) };
		}

		stream = SDL_CreateAudioStream(&spec, nullptr);
		if (!stream)
		{
			throw std::runtime_error{ std::format("Could not create an audio stream: {}", SDL_GetError()) };
		}
		if (!SDL_BindAudioStream(audio_device, stream))
		{
			throw std::runtime_error{ std::format("Failed to bind audio stream: {}", SDL_GetError()) };
		}
	}

	Sound::~Sound()
	{
		SDL_free(wav_data);
		SDL_DestroyAudioStream(stream);
	}

	void Sound::setVolume(float percentage)
	{
		SDL_SetAudioStreamGain(stream, percentage);
	}

	void Sound::play()
	{
		if (play_type == PlayType::STOP_ON_PLAY)
			SDL_ClearAudioStream(stream);
		
		if (SDL_GetAudioStreamQueued(stream) < static_cast<int>(wav_data_len))
			SDL_PutAudioStreamData(stream, wav_data, static_cast<int>(wav_data_len));
	}

	void Sound::stop()
	{
		SDL_ClearAudioStream(stream);
	}
}
