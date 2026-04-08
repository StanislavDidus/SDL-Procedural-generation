#include "World.hpp"
#include "TileManager.hpp"

World::World(const Grid<Tile>& grid, const std::vector<ObjectData>& objects, const std::vector<ChestData>& chests)
	: grid{grid}
	, objects{objects}
	, chests{chests}
{
	sprite_map.resize(grid.getColumns() * grid.getRows());

	updateSpriteMap();
}

const std::vector<Uint32>& World::getSpriteMap() const
{
	return sprite_map;
}

void World::updateSpriteMap()
{
	for (int y = 0; y < grid.getRows(); ++y)
	{
		for (int x = 0; x < grid.getColumns(); ++x)
		{
			int sprite_index = TileManager::get().getProperties(grid(x, y).id).sprite_index;
			sprite_map[x + y * grid.getColumns()] = sprite_index;
		}
	}
}
