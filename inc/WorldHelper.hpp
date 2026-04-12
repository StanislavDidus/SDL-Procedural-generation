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

static Entity spawnObject(entt::registry& registry, const ObjectData& object_data, float tile_width, float tile_height)
{
	const auto& tile_set = ResourceManager::get().getSpriteSheet("objects");

	auto entity = registry.create();
	const auto& rect = object_data.grid_rect;
	auto& ts = registry.emplace<Components::Transform>(entity);
	ts.position.x = rect.x * tile_width;
	ts.position.y = rect.y * tile_height;
	ts.size.x = rect.w * tile_width;
	ts.size.y = rect.h * tile_height;

	const auto& sprite = tile_set->getSprite(ObjectManager::get().getProperties(object_data.properties_id).sprite_index);
	auto& renderable = registry.emplace<Components::Renderable>(entity);
	renderable.sprite = sprite;

	size_t object_id = object_data.properties_id;
	float maximum_durability = ObjectManager::get().getProperties(object_id).durability;
	auto& object_component = registry.emplace<Components::Object>(entity, object_id);
	object_component.current_durability = maximum_durability;

	return entity;
}

static Entity spawnChest(entt::registry& registry, const ChestData& chest_data, float tile_width, float tile_height)
{
	auto chest = registry.create();
	auto& ts = registry.emplace<Components::Transform>(chest);
	ts.position.x = chest_data.grid_rect.x * tile_width;
	ts.position.y = chest_data.grid_rect.y * tile_height;
	ts.size.x = chest_data.grid_rect.w * tile_width;
	ts.size.y = chest_data.grid_rect.h * tile_height;

	const auto& sprite = ResourceManager::get().getSpriteSheet("objects")->getSprite("Chest");
	auto& renderable = registry.emplace<Components::Renderable>(chest);
	renderable.sprite = sprite;

	auto& chest_component = registry.emplace<Components::Chest>(chest);
	chest_component.base_item = chest_data.base_item;
	chest_component.common_item = chest_data.common_item;
	chest_component.sand_item = chest_data.sand_item;
	chest_component.snow_item = chest_data.snow_item;

	registry.emplace<Components::Button>(chest, true);

	return chest;
}

static void damageTile(Grid<Tile>& grid, int tile_x, int tile_y, float damage)
{
	float world_width_tiles = grid.getColumns();
	float world_height_tiles = grid.getRows();

	if (tile_x < 0 || tile_x > world_width_tiles - 1 || tile_y < 0 || tile_y > world_height_tiles - 1) return;

	auto& tile = grid(tile_x, tile_y);

	if (auto& tile_properties = TileManager::get().getProperties(tile.id); tile_properties.is_solid && !tile.attached)
	{
		tile.dealDamage(damage);

		if (tile.is_destroyed)
		{
			tile.id = TileManager::get().getTileID("Sky");
			tile.is_destroyed = false;
		}
	}
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


static void placeTile(Grid<Tile>& grid, int tile_x, int tile_y, size_t tile_id)
{
	float world_width_tiles = grid.getColumns();
	float world_height_tiles = grid.getRows();

	if (tile_x < 0 || tile_x > world_width_tiles - 1 || tile_y < 0 || tile_y > world_height_tiles - 1) return;

	auto& tile = grid(tile_x, tile_y);
	auto& tile_manager = TileManager::get();

	if (auto& tile_properties = tile_manager.getProperties(tile.id); !tile_properties.is_solid)
	{
		//Check adjacent tiles
		bool is_placement_allowed = false;
		for (int i = -1; i < 2; ++i) for (int j = -1; j < 2; ++j)
		{
			//Don't check the middle tile (it is where we want to place a new tile)
			if (i == 0 && j == 0) continue;

			int new_x = tile_x + i;
			int new_y = tile_y + j;

			if (new_x < 0 || new_x >= world_width_tiles || new_y < 0 || new_y >= world_height_tiles) continue;

			auto& tile_temp = grid(new_x, new_y);
			auto& properties_temp = tile_manager.getProperties(tile_temp.id);
			if (properties_temp.is_solid)
			{
				is_placement_allowed = true;
				break;
			}
		}

		if (is_placement_allowed)
		{
			tile.id = tile_id;
		}
	}
}

static void updateTilesDurability(Grid<Tile>& grid)
{
	float world_width_tiles = grid.getColumns();
	float world_height_tiles = grid.getRows();

	const auto& tile_manager = TileManager::get();
	for (int x = 0; x < world_width_tiles; ++x)
	{
		for (int y = 0; y < world_height_tiles; ++y)
		{
			auto& tile = grid(x, y);

			if (tile.received_damage_last_frame == false)
			{
				tile.current_durability = tile_manager.getProperties(tile.id).max_durability;
			}
			tile.received_damage_last_frame = false;
		}
	}
}
