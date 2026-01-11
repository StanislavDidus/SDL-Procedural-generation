#pragma once

#include <iostream>
#include <string>

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

	int surface_tile;
	int dirt_tile;
};