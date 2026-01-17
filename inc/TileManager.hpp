#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "Tile.hpp"

class TileManager
{
public:
	TileManager() = default;

	void loadXml(const std::filesystem::path& path);

	size_t registerTile(const TileProperties& properties); // Returns an index to the tile in a vector

	//Getters
	size_t getTileID(const std::string& tile_name) const;
	const TileProperties& getProperties(int ID) const;
private:
	std::vector<TileProperties> tiles;
	std::unordered_map<std::string, size_t> tileNameToID;///< Map that returns an <b>ID</b> of a tile with the specified name.
	size_t tiles_count = 0;
};