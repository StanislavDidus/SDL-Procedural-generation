#pragma once

#include <vector>

#include "Tile.hpp"

struct Chunk
{
	Chunk();
	Chunk(int index_x, int index_y, int x, int y);
	~Chunk() = default;

	void addTile(int index, int row, int column);

	int index_x;
	int index_y;
	int x;
	int y;

	std::vector<Tile> tiles;
};