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
	bool is_solid;
	float max_durability;
};

struct Tile
{
	Tile() = default;

	int id = 0;

	float current_durability = 0.f;
	bool is_destroyed = false;
	bool sealed = false;
	bool received_damage_last_frame = false;

	void dealDamage(float damage);

#ifdef DEBUG_TILES
	float pv = 0.f;
	float temperature = 0.f;
	float moisture = 0.f;
#endif

	bool operator==(const Tile& other) const { return this->id == other.id; }
	
};