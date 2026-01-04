#include "Chunk.hpp"

Chunk::Chunk() : index_x(0), index_y(0), x(0), y(0)
{
}

Chunk::Chunk(int index_x, int index_y, int x, int y) : index_x(index_x), index_y(index_y), x(x), y(y)
{
}

void Chunk::addTile(const Tile& tile)
{
	tiles.push_back(tile);
}

void Chunk::addObject(const Object& object)
{
	objects.push_back(object);
}


