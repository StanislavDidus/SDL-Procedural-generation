#pragma once

#include <iostream>
#include "glm/glm.hpp"

static float pcg_hash(uint32_t input)
{
	uint32_t state = input * 747796405u + 2891336453u;
	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return static_cast<float>((word >> 22u) ^ word);
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

class Noise
{
public:
	Noise();
	virtual ~Noise();

	virtual float noise1D(float x, uint32_t seed) const = 0;
	virtual float noise2D(float x, float y, uint32_t seed) const = 0;

	template<typename NoiseType>
	static float fractal1D(const NoiseType& noise, int octaves, float x, float frequency, float amplitude, uint32_t seed) 
	{
		float output = 0.f;
		float denom = 0.f;
		float frequency_ = frequency;
		float amplitude_ = amplitude;

		for (int i = 0; i < octaves; ++i)
		{
			output += noise.noise1D(x * frequency_, seed) * amplitude_;
			denom += amplitude_;

			frequency_ *= 2.f;
			amplitude_ *= .5f;
		}

		return output / denom;
	}
};