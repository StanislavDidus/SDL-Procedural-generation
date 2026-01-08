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

class Tile
{
public:
	Tile() : sprite_index(0), local_x(0), local_y(0), type(TileType::NONE), max_durability(1.f) {}
	Tile(int sprite_index, TileType type, bool solid, float max_durability) : sprite_index(sprite_index), local_x(0), local_y(0), type(type), solid(solid), max_durability(max_durability), current_durability(max_durability) {}
	Tile(int sprite_index, int local_x, int local_y, TileType type, bool solid, float max_durability) : sprite_index(sprite_index), local_x(local_x), local_y(local_y), type(type), solid(solid), max_durability(max_durability), current_durability(max_durability) {}

	void setTile(const Tile& tile)
	{
		this->sprite_index = tile.sprite_index;
		this->type = tile.type;
		this->solid = tile.solid;
		this->max_durability = tile.max_durability;
		this->current_durability = this->max_durability;
		is_destroyed = false;
	}

	void dealDamage(float damage);

	int sprite_index;
	int local_x;
	int local_y;
	TileType type;

	float max_durability = 0.f;
	float current_durability = 0.f;
	bool is_destroyed = false;

	bool solid = false;
	bool sealed = false;

	bool received_damage_last_frame = false;

#ifdef DEBUG_TILES
	float pv = 0.f;
	float temperature = 0.f;
	float moisture = 0.f;
#endif

	bool operator==(const Tile& other) const { return this->sprite_index == other.sprite_index && this->type == other.type; }
	
};