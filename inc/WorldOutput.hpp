#pragma once
#include <vector>

#include "Grid.hpp"
#include "Object.hpp"
#include "Tile.hpp"

// Information about objects that will be spawned

struct ChestData
{
	SDL_Rect grid_rect;

	size_t base_item;
	size_t common_item;
	size_t snow_item;
	size_t sand_item;
};

struct ObjectData
{
	SDL_Rect grid_rect;
	size_t properties_id;
};

struct WorldOutput
{
	Grid<Tile> grid;
	std::vector<ObjectData> objects;
	std::vector<ChestData> chests;
};
