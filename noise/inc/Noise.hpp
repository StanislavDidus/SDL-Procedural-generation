#pragma once

#include <iostream>
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

static float pcg_hash(uint32_t input)
{
	uint32_t state = input * 747796405u + 2891336453u;
	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return static_cast<float>((word >> 22u) ^ word);
}

static float getWholeHash(uint32_t x, uint32_t y, uint32_t seed)
{
	uint32_t h = x;
	h ^= y * 0x9E3779B9u;
	h ^= seed * 0x85EBCA6Bu;
	return pcg_hash(h);
}

static float getHashValue(uint32_t x, uint32_t seed)
{
	//uint32_t input = x * 374761393u;
	//return pcg_hash(input) / 4'294'967'295.0f;

	uint32_t h = x;
	h ^= seed * 0x9E3779B9u;
	return pcg_hash(h) / 4'294'967'295.0f;
}

static float getHashValue(uint32_t x, uint32_t y, uint32_t seed)
{
	//uint32_t input = x * 374761393u + y * 668265263u;
	//return pcg_hash(input) / 4'294'967'295.0f;

	uint32_t h = x;
	h ^= y * 0x9E3779B9u;    
	h ^= seed * 0x85EBCA6Bu;
	return pcg_hash(h) / 4'294'967'295.0f;
}

static float smoothStep(float value)
{
	return value * value * (3.f - 2.f * value);
}

struct NoiseSettings
{
	int octaves;
	float frequency;
	float amplitude;
	uint32_t seed;
};

class Noise
{
public:
	Noise() = default;
	~Noise() = default;

	//Fbm - fractal brownian motion

	template<typename NoiseType>
	static float fractal1D(int octaves, float x, float frequency, float amplitude, uint32_t seed) 
	{
		float output = 0.f;
		//float denom = 0.f;
		float frequency_ = frequency;
		float amplitude_ = amplitude;
		float lacunarity = 2.f;
		float gain = 0.5f; // persistence

		for (int i = 0; i < octaves; ++i)
		{
			output += NoiseType::noise1D(x * frequency_, seed) * amplitude_;

			frequency_ *= lacunarity;
			//denom += amplitude_;
			amplitude_ *= gain;
		}

		//return std::clamp(output, 0.f, 1.f);
		return output;
	}

	template<typename NoiseType>
	static float fractal1D(const NoiseSettings& settings, float x)
	{
		return fractal1D<NoiseType>(settings.octaves, x, settings.frequency, settings.amplitude, settings.seed);
	}

	template<typename NoiseType>
	static float fractal2D(int octaves, float x, float y, float frequency, float amplitude, uint32_t seed)
	{
		float output = 0.f;
		float denom = 0.f;
		float frequency_ = frequency;
		float amplitude_ = amplitude;
		float lacunarity = 2.f;
		float gain = 0.5f; // persistence

		for (int i = 0; i < octaves; ++i)
		{
			output += NoiseType::noise2D(x * frequency_, y * frequency_, seed) * amplitude_;

			frequency_ *= lacunarity;
			denom += amplitude_;
			amplitude_ *= gain;
		}

		//return std::clamp(output, 0.f, 1.f);
		return output / denom;
	}

	template<typename NoiseType>
	static float fractal2D(const NoiseSettings& settings, float x, float y)
	{
		return fractal2D<NoiseType>(settings.octaves, x, y, settings.frequency, settings.amplitude, settings.seed);
	}
};