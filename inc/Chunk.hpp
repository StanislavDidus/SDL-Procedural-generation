#pragma once

#include <vector>

#include <SDL3/SDL_rect.h>
#include "ECS/Entity.hpp"
#include "VertexBuffer.hpp"
#include "glm/vec2.hpp"

inline SDL_FPoint& operator+=(SDL_FPoint& lhs, const SDL_FPoint& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

inline SDL_FPoint& operator-=(SDL_FPoint& lhs, const glm::vec2& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

struct Chunk
{
	Chunk() = default;
	Chunk(const SDL_FRect& rect, const glm::ivec2& index) : rect{rect}, index{index}
	{
	}

	~Chunk() = default;
	
	SDL_FRect rect{};
	glm::ivec2 index{};

	std::vector<Entity> objects; // object player can mine and chest to loot
};