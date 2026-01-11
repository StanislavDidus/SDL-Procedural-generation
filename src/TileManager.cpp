#include "TileManager.hpp"

const TileProperties& TileManager::getProperties(int id) const
{
	return tiles[id];
}

int TileManager::addTile(const TileProperties& properties)
{
	tiles.push_back(properties);
	return tiles.size() - 1;
}
