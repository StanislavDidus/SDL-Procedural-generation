#pragma once

#include "Tile.hpp"
#include "Grid.hpp"

#include <functional>

#include "ResourceManager.hpp"
#include "World.hpp"
#include "ECS/Components.hpp"

//This is a "WorldHelper" class that introduces static function that "WorldGenerator" class uses for its world_generator generation.

static int mapRange(int x, int inMin, float inMax, int outMin, int outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

//Returns true if a grid_rect is completely inside b
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
		if (x + i < 0 || x + i >= grid.getColumns() || y < 0 || y >= grid.getRows())
			return false;

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
			//Check the boundaries of thw world_generator before getting a tile from the world_map
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

static Entity getObjectOnPosition(const entt::registry& registry, const glm::vec2& mouse_global_position)
{
	SDL_FRect mouse_rect
	{
		mouse_global_position.x,
		mouse_global_position.y,
		1.0f,
		1.0f
	};

	auto view = registry.view<Components::Transform, Components::Object>();
	for (auto [entity, transform_component, object_component] : view.each())
	{
		SDL_FRect object_rect
		{
			transform_component.position.x,
			transform_component.position.y,
			transform_component.size.x,
			transform_component.size.y
		};

		if (SDL_HasRectIntersectionFloat(&mouse_rect, &object_rect))
		{
			return entity;
		}
	}

	return entt::null;
}

static std::optional<Components::Object> damageObject(entt::registry& registry, const glm::vec2& mouse_global_position, float damage)
{
	SDL_FRect mouse_rect
	{
		mouse_global_position.x,
		mouse_global_position.y,
		1.0f,
		1.0f
	};

	Entity entity = getObjectOnPosition(registry, mouse_global_position);
	if (entity != entt::null)
	{
		auto& object_component = registry.get<Components::Object>(entity);
		object_component.current_durability -= damage;
		object_component.received_damage_last_frame = true;

		if (object_component.current_durability <= 0.0f && !object_component.is_destroyed)
		{
			object_component.is_destroyed = true;
			return object_component;
		}
	}
	return std::nullopt;
}
