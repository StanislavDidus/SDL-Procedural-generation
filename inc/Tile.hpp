#pragma once

enum class TileType
{
	NONE,
	WATER,
	SURFACE,
	DIRT,
	STONE,
};


struct TileDebugInfo
{
	float pv = 0.f;
	float temperature = 0.f;
	float moisture = 0.f;
};

struct Tile
{
	Tile(int index, int x, int y, TileType type = TileType::NONE) : index(index), x(y), y(y), type(type) {}

	int index;
	int x;
	int y;
	TileType type;

	bool solid = false;
	bool sealed = false;

	TileDebugInfo debug_info;
};