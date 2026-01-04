#pragma once

#include <vector>

#include "Object.hpp"
#include "Tile.hpp"

struct Chunk
{
	Chunk();
	Chunk(int index_x, int index_y, int x, int y);
	~Chunk() = default;

	void addTile(const Tile& tile);
	void addObject(const Object& object);

	int index_x;
	int index_y;
	int x;
	int y;

	std::vector<Object> objects;
	std::vector<Tile> tiles;

	bool operator==(const Chunk& other) const { return this->index_x == other.index_x && this->index_y == other.index_y; }
};