#pragma once

#include <vector>
#include <map>

#include <SDL3/SDL_rect.h>
#include "Vec2Less.hpp"
#include "TileMap.hpp"

struct Chunk
{
	Chunk(const SpriteSheet& tileset, const SDL_FRect& rect, int rows, int columns);
	~Chunk() = default;
	
	SDL_FRect rect;
	TileMap tilemap;
};