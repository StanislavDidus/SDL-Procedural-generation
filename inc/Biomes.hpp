#pragma once

#include <iostream>
#include <string>

#include "Renderer.hpp"
#include "glm/glm.hpp"
#include "Noise.hpp"
#include "Tile.hpp"

static float between(float value, float min, float max)
{
	return value >= min && value <= max;
}

class Biome
{
public:
	Biome() = default;

	std::string name;

	float pv_min;
	float pv_max;

	float temperature_min;
	float temperature_max;

	float moisture_min;
	float moisture_max;

	Tile surface_tile;
	Tile dirt_tile;
};