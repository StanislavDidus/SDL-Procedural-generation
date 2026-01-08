#pragma once

#include "Debug.hpp"

enum class TileType
{
	NONE,
	WATER,
	SURFACE,
	DIRT,
	STONE,
};

struct TileProperties
{
	int sprites_index;
	TileType type;
	float max_durability;
	bool is_solid;
};

struct Tile
{
	Tile() = default;

	int id;

	float current_durability = 0.f;
	bool is_destroyed = false;
	bool sealed = false;
	bool received_damage_last_frame = false;

#ifdef DEBUG_TILES
	float pv = 0.f;
	float temperature = 0.f;
	float moisture = 0.f;
#endif

	bool operator==(const Tile& other) const { return this->id == other.id; }
	
};