#pragma once

#include <iostream>
#include <string>

#include "Renderer.hpp"
#include "glm/glm.hpp"

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

	float frequency;
	float amplitude;
	float seed;

	float strength;
	float height_multiplier;

	float weight(float temperature_, float moisture_) const;
};