#pragma once

#include <vector>

#include <SDL3/SDL_rect.h>
#include "TileMap.hpp"
#include "Object.hpp"

struct Chunk
{
	Chunk(const SpriteSheet& tileset, const SDL_FRect& rect, int rows, int columns);
	~Chunk() = default;
	
	SDL_FRect rect;
	Grid<Tile> tilemap;
	std::vector<Object> objects;
};