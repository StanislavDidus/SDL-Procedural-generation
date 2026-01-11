#pragma once

#include <vector>
#include "Tile.hpp"

class TileManager
{
public:
	TileManager() = default;
	~TileManager() = default;

	const TileProperties& getProperties(int index) const;
	int addTile(const TileProperties& properties); // Returns an index to the tile in a vector
private:
	std::vector<TileProperties> tiles;
};