#pragma once

#include "Tile.hpp"
#include "Grid.hpp"

#include <functional>

//This is a "WorldHelper" class that introduces static function that "World" class uses for its world generation.

static int mapRange(int x, int inMin, float inMax, int outMin, int outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

//Returns true if a rect is completely inside b
static bool isRectInsideFloat(const SDL_FRect& a, const SDL_FRect& b)
{
	return a.x >= b.x &&
		a.x + a.w <= b.x + b.w &&
		a.y >= b.y &&
		a.y + a.h <= b.y + b.h;
}

//Check tiles if they meet some condition
static bool checkTileFloor(const Grid<Tile>& grid, int x, int y, int range, const std::function<bool(const Tile&)>& predicament)
{
	for (int i = 0; i < range; ++i)
	{
		auto& new_tile = grid(x + i, y);
		if (!predicament(new_tile))
		{
			return false;
		}
	}

	return true;
}

//Check if space above the given tile is free
static bool checkTileSpace(const Grid<Tile>& grid, int x, int y, int w, int h)
{
	for (int i = 0; i < w; ++i)
	{
		//Set a small 1 tile offset for y loop because we want to check tiles above the ground
		for (int j = 1; j <= h; ++j)
		{
			//Check the boundaries of thw world before getting a tile from the world_map
			int new_x = x + i;
			int new_y = y - j;

			if (new_x < 0 || new_x >= grid.getColumns() || new_y < 0 || new_y >= grid.getRows())
			{
				return false;
			}

			auto& new_tile = grid(x + i, y - j);

			if (TileManager::get().getProperties(new_tile.id).is_solid)
			{
				return false;
			}
		}
	}

	return true;
}
