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

struct Tile
{
	Tile() : sprite_index(0), local_x(0), local_y(0), type(TileType::NONE), durability(0.f) {}
	Tile(int sprite_index, TileType type, bool solid, float durability) : sprite_index(sprite_index), local_x(0), local_y(0), type(type), solid(solid), durability(durability) {}
	Tile(int sprite_index, int local_x, int local_y, TileType type, bool solid, float durability) : sprite_index(sprite_index), local_x(local_x), local_y(local_y), type(type), solid(solid), durability(durability) {}

	void setTile(const Tile& tile)
	{
		this->sprite_index = tile.sprite_index;
		this->type = tile.type;
		this->solid = tile.solid;
		this->durability = tile.durability;
	}

	int sprite_index;
	int local_x;
	int local_y;
	TileType type;

	float durability;

	bool solid = false;
	bool sealed = false;

#ifdef DEBUG_TILES
	float pv = 0.f;
	float temperature = 0.f;
	float moisture = 0.f;
#endif
};