#include "Sound.hpp"
#include <exception>
#include <format>

#include "SDL3/SDL_filesystem.h"

namespace audio
{
	Sound::Sound(SDL_AudioDeviceID audio_device, const std::filesystem::path& path)
		: audio_device{audio_device}
	{
		if (!SDL_LoadWAV(path.string().c_str(), &spec, &wav_data, &wav_data_len))
		{
			throw std::runtime_error{ std::format("Could not load .wav file: {}", SDL_GetError()) };
		}
	}

	Sound::~Sound()
	{
		SDL_free(wav_data);
	}

	void Sound::setVolume(float percentage)
	{
		volume = percentage;
	}

	void Sound::play()
	{
		SDL_AudioStream* stream = SDL_CreateAudioStream(&spec, nullptr);
		if (!stream)
		{
			throw std::runtime_error{ std::format("Could not create an audio stream: {}", SDL_GetError()) };
		}
		if (!SDL_BindAudioStream(audio_device, stream))
		{
			throw std::runtime_error{ std::format("Failed to bind audio stream: {}", SDL_GetError()) };
		}

		SDL_SetAudioStreamGain(stream, volume);
		SDL_PutAudioStreamData(stream, wav_data, static_cast<int>(wav_data_len));
		streams.push_back(stream);
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void Sound::update()
	{
		for (const auto& stream : streams)
		{
			if (SDL_GetAudioStreamAvailable(stream) == 0)
			{
				SDL_DestroyAudioStream(stream);
			}
		}
	}
}
