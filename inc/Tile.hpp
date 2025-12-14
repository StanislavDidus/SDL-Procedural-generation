#pragma once

enum class TileType
{
	NONE,
	SOLID,
	AIR,
	WATER,
	SURFACE,
	DIRT,
};

struct Tile
{
	Tile(int index, int row, int column, TileType type) : index(index), row(row), column(column), type(type) {}

	int index;
	int row;
	int column;
	TileType type;
	bool sealed = false;
	
};