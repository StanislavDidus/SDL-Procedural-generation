#include "Biomes.hpp"
#include <algorithm>

Biome::Biome(const std::string& name, float pv_min, float pv_max, float temperature_min, float temperature_max,
	float moisture_min, float moisture_max, size_t surface_tile_id, size_t dirt_tile_id) : name(name), pv_min(pv_min),
	pv_max(pv_max), temperature_min(temperature_min), temperature_max(temperature_max), moisture_min(moisture_min),
	moisture_max(moisture_max), surface_tile_id(surface_tile_id), dirt_tile_id(dirt_tile_id)
{
}
