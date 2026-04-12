#pragma once
#include <vector>
#include <entt/entity/registry.hpp>

#include "Grid.hpp"
#include "Object.hpp"
#include "Tile.hpp"
#include "ECS/Entity.hpp"

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

class World
{
public:
	World(const Grid<Tile>& grid, const std::vector<ObjectData>& objects, const std::vector<ChestData>& chests);
	~World() = default;

	void initWorld(entt::registry& registry, float tile_width, float tile_height);

	// Getters
	const std::vector<Uint32>& getSpriteMap() const;

	void update(entt::registry& registry);
	void updateSpriteMap();

	Grid<Tile> grid;
	std::vector<ObjectData> objects;
	std::vector<ChestData> chests;

	std::vector<Entity> object_entities;
	std::vector<Entity> chest_entities;
private:
	void spawnObjects(entt::registry& registry, float tile_width, float tile_height);
	void spawnChests(entt::registry& registry, float tile_width, float tile_height);
	
	std::vector<Uint32> sprite_map;
};

