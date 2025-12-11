#pragma once

#include <iostream>
#include <string>

#include "Renderer.hpp"
#include "glm/glm.hpp"
#include "Noise.hpp"

static float between(float value, float min, float max)
{
	return value >= min && value <= max;
}

class Biome
{
public:
	Biome() = default;

	std::string name;
	float temperature;
	float moisture;
	int tile_id;

	int octaves;
	float frequency;
	float amplitude;
	uint32_t seed;

	float strength;
	float height_multiplier;

	template<typename NoiseType>
	float height(float x) const
	{
		return Noise::fractal1D<NoiseType>(octaves, x, frequency, amplitude, seed) * height_multiplier;
	}

	float weight(float temperature_, float moisture_) const;
};