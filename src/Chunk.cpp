#include "Chunk.hpp"

Chunk::Chunk() : index(0), x(0.f), y(0.f)
{
}

Chunk::Chunk(int index, float x, float y) : index(index), x(x), y(y)
{
}

void Chunk::addTile(int index, int row, int column)
{
	tiles.emplace_back(index, row, column);
}


