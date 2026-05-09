#pragma once
#include <vector>
#include <entt/entity/registry.hpp>

#include <graphics/GpuRenderer.hpp>
#include <graphics/TileMap.hpp>
#include "Grid.hpp"
#include "Object.hpp"
#include "Tile.hpp"
#include "ECS/Entity.hpp"
#include "ECS/ShowMessageSystem.hpp"

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

class Game;

class World
{
public:
	struct DamagedTile
	{
		float durability_percentage;
		glm::ivec2 grid_position;
	};
	
	struct Chunk
	{
		bool is_dirty = true;	
		bool damaged_tile = false;
	};
	
	World(const Grid<Tile>& grid, const std::vector<PortalData>& portals, const std::vector<ObjectData>& objects, const std::vector<ChestData>& chests);
	~World() = default;

	void initWorld(entt::registry& registry, float tile_width, float tile_height);
	void spawnPortals(entt::registry& registry, std::shared_ptr<ShowMessageSystem> show_message_system, Entity player, float tile_width, float tile_height, Game
	                  * game, std::shared_ptr<graphics::SpriteAnimation> portal_animation);

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
	void spawnObjects(entt::registry& registry, float tile_width, float tile_height);
	void spawnChests(entt::registry& registry, float tile_width, float tile_height);

	int getChunkIndexByTilePosition(int x, int y) const;
	static void updateObjectsDurability(entt::registry& registry);
	void updateTilesDurability();
	
	std::vector<DamagedTile> damaged_tiles;
	
	std::vector<Uint32> sprite_map;
	std::vector<Chunk> chunks;
	float chunk_width_tiles = 50.0f;
	float chunk_height_tiles = 50.0f;
};

