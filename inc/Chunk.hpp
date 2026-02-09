#pragma once

#include <vector>

#include <SDL3/SDL_rect.h>
#include "TileMap.hpp"
#include "Object.hpp"

struct Chunk
{
	Chunk(const SDL_FRect& rect, const glm::ivec2& grid_position);
	~Chunk() = default;
	
	SDL_FRect rect;
	glm::ivec2 grid_position;
	std::vector<Object> objects;
};