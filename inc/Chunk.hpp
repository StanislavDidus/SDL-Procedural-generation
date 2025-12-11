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

	bool operator==(const Chunk& other) const { return this->index_x == other.index_x && this->index_y == other.index_y; }
};