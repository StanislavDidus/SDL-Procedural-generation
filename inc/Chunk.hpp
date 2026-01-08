#pragma once

#include <vector>
#include <map>

#include "Object.hpp"
#include "glm/vec2.hpp"
#include "Tile.hpp"
#include "Vec2Less.hpp"

struct Chunk
{
	Chunk();
	Chunk(int index_x, int index_y, int x, int y);
	~Chunk() = default;

	void addTile(const Tile& tile);
	void addObject(const glm::ivec2& position, const Object& object);

	int index_x;
	int index_y;
	int x;
	int y;

	std::map<glm::ivec2, Object, Vec2Less> objects;
	std::vector<Tile> tiles;


	bool operator==(const Chunk& other) const { return this->index_x == other.index_x && this->index_y == other.index_y; }
};