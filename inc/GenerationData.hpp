#pragma once

#include <unordered_map>
#include "Biomes.hpp"
#include "Noise.hpp"
#include "MapRange.hpp"
#include "RandomizedItem.hpp"

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
	CHEST,
	LOOT,
	DRUNK_WALKER,
	DRUNK_WALKER_MOVEMENT,
};

enum class MapRangeType
{
	PV_HEIGHT,
	PV_CHANGE,
	CAVE_CHANGE,
	CAVE_SIZE_CHANGE,
};

enum class LootType
{
	BASE,
	COMMON,
	SNOW,
	SAND,
};

struct GenerationData
{
	std::vector<Biome> biomes;
	std::unordered_map<NoiseType, NoiseSettings> noise_settings;
	std::unordered_map<MapRangeType, MapRange> maps;

	glm::vec2 chest_size = {2,2};

	// Chest Loot
	std::unordered_map<LootType, std::vector<RandomizedItem>> chest_loot;

	float scale = 0.1f;
	float y_base = 0.f;
	float cave_y_base = 0.f;
	float sea_y_base = 0.f;
	float density_threshold = 0.5f;
	float chest_threshold = 0.5f;
	float drunk_walker_threshold = 0.3f;
};