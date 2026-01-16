#pragma once

#include <unordered_map>
#include "Biomes.hpp"
#include "Noise.hpp"

class TileManager;
class ObjectManager;

enum class BlockType
{
	GRASS,
	DIRT,
	STONE,

	SNOW_GRASS,
	SNOW_DIRT,

	SAND,

	ROCK,

	SKY,
	WATER,
};

enum class BiomeType
{
	FOREST,
	DESERT,
	TUNDRA,
};

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
	std::unordered_map<BlockType, int> tiles;
	std::unordered_map<BiomeType, Biome> biomes;
	std::unordered_map<NoiseType, NoiseSettings> noise_settings;
	std::shared_ptr<TileManager> tile_manager;
	std::shared_ptr<ObjectManager> object_manager;
	std::unordered_map<MapRangeType, MapRange> maps;

	float scale;
	float y_base = 0.f;
	float cave_y_base = 0.f;
	float sea_y_base = 0.f;
	float density_threshold;
};