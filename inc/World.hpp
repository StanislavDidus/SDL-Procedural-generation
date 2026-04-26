#pragma once
#include <vector>
#include <entt/entity/registry.hpp>

#include "GpuRenderer.hpp"
#include "TileMap.hpp"
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

struct PortalData
{
	SDL_FRect grid_rect;
};

class World
{
public:
	struct DamagedTile
	{
		float durability_percentage;
		glm::ivec2 grid_position;
	};
	
	World(const Grid<Tile>& grid, const std::vector<PortalData>& portals, const std::vector<ObjectData>& objects, const std::vector<ChestData>& chests);
	~World() = default;

	void initWorld(entt::registry& registry, float tile_width, float tile_height);

	void placeTile(int x, int y, int tile_id);
	void damageTile(int x, int y, float damage);

	// Getters
	const std::vector<Uint32>& getSpriteMap() const;

	void update(entt::registry& registry);
	void render(graphics::GpuRenderer& screen, float tile_width_world, float tile_height_world) const;
	void setSpriteMap(graphics::TileMap& tilemap);

	Grid<Tile> grid;
	std::vector<PortalData> portals;
	std::vector<ObjectData> objects;
	std::vector<ChestData> chests;

	std::vector<Entity> object_entities;
	std::vector<Entity> chest_entities;
private:
	void spawnPortals(entt::registry& registry, float tile_width, float tile_height);
	void spawnObjects(entt::registry& registry, float tile_width, float tile_height);
	void spawnChests(entt::registry& registry, float tile_width, float tile_height);

	static void updateObjectsDurability(entt::registry& registry);
	void updateTilesDurability();
	
	std::vector<DamagedTile> damaged_tiles;
	
	std::vector<Uint32> sprite_map;
	bool is_dirty = true;
};

