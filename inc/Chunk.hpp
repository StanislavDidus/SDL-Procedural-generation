#pragma once

#include <vector>

#include <SDL3/SDL_rect.h>
#include "TileMap.hpp"
#include "Object.hpp"
#include "VertexBuffer.hpp"

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

template<int WidthTiles, int HeightTiles>
struct Chunk
{
	Chunk() = default;
	Chunk(const SDL_FRect& rect, const glm::ivec2& grid_position) : rect{rect}, grid_position{grid_position}
	{
		const SDL_FPoint tile_size = {rect.w / WidthTiles, rect.h / HeightTiles};
		for (int i = 0; i < WidthTiles * HeightTiles; ++i)
		{
			int x = i % WidthTiles;
			int y = i / WidthTiles;

			const SDL_FPoint top_left
			{
				rect.x + x * tile_size.x,
				rect.y + y * tile_size.y
			};

			SDL_FPoint p{ top_left.x, top_left.y };
			SDL_FPoint p1{ top_left.x + tile_size.x, top_left.y };
			SDL_FPoint p2{ top_left.x + tile_size.x, top_left.y + tile_size.y};
			SDL_FPoint p3{ top_left.x, top_left.y + tile_size.y };

			SDL_FColor color{ 1.0f, 1.0f, 1.0f, 1.0f };

			vertices[i * 4 + 0] = SDL_Vertex{ p, color, {} };
			vertices[i * 4 + 1] = SDL_Vertex{ p1, color, {} };
			vertices[i * 4 + 2] = SDL_Vertex{ p2, color, {} };
			vertices[i * 4 + 3] = SDL_Vertex{ p3, color, {} };
		}
	}

	~Chunk() = default;
	
	SDL_FRect rect{};
	glm::ivec2 grid_position{};

	std::vector<Object> objects;
	std::array<SDL_Vertex, WidthTiles * HeightTiles * 4> vertices;
};