#pragma once

#include <vector>

#include "Tile.hpp"

struct Chunk
{
	Chunk();
	Chunk(int index_x, int index_y, float x, float y);
	~Chunk() = default;

	void addTile(int index, int row, int column);

	int index_x;
	int index_y;
	float x;
	float y;

	std::vector<Tile> tiles;
};