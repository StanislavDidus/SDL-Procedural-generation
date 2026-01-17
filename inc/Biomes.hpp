#pragma once

#include <string>

static float between(float value, float min, float max)
{
	return value >= min && value <= max;
}

class Biome
{
public:
	Biome() = default;

	Biome(const std::string& name, float pv_min, float pv_max, float temperature_min, float temperature_max,
		float moisture_min, float moisture_max, size_t surface_tile_id, size_t dirt_tile_id);

	std::string name;

	float pv_min;
	float pv_max;

	float temperature_min;
	float temperature_max;

	float moisture_min;
	float moisture_max;

	size_t surface_tile_id;
	size_t dirt_tile_id;
};