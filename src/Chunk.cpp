#include "Chunk.hpp"

Chunk::Chunk() : index_x(0), index_y(0), x(0.f), y(0.f)
{
}

Chunk::Chunk(int index_x, int index_y, float x, float y) : index_x(index_x), index_y(index_y), x(x), y(y)
{
}

void Chunk::addTile(int index, int row, int column)
{
	tiles.emplace_back(index, row, column);
}


