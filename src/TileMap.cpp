#include "TileMap.hpp"

using namespace graphics;

TileMap::TileMap(const SpriteSheet& tileset, int rows, int columns) 
	: grid(rows, columns)
	, tileset(tileset)
{
}

TileMap::~TileMap()
{
}

int TileMap::getRows() const
{
	return grid.getRows();
}

int TileMap::getColumns() const
{
	return grid.getColumns();
}

const SpriteSheet& TileMap::getSpriteSheet() const
{
	return tileset;
}
