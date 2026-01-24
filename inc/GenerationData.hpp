#pragma once

#include <unordered_map>
#include "Biomes.hpp"
#include "Noise.hpp"
#include "MapRange.hpp"

class TileManager;
class ObjectManager;

enum class NoiseType
{
	PV,
	DENSITY,
	DIRT,
	CAVE,
	TUNNEL,
	TEMPERATURE,
	MOISTURE,
};

enum class MapRangeType
{
	PV_HEIGHT,
	PV_CHANGE,
	CAVE_CHANGE,
};

struct GenerationData
{
	std::vector<Biome> biomes;
	std::unordered_map<NoiseType, NoiseSettings> noise_settings;
	std::unordered_map<MapRangeType, MapRange> maps;

	float scale;
	float y_base = 0.f;
	float cave_y_base = 0.f;
	float sea_y_base = 0.f;
	float density_threshold;
};