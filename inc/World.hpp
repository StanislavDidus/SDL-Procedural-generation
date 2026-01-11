#pragma once

#include "glm/vec2.hpp"

#include <memory>
#include <array>
#include <map>
#include <optional>

#include "Biomes.hpp"
#include "ValueNoise.hpp"
#include "PerlynNoise.hpp"
#include "Chunk.hpp"
#include "MapRange.hpp"
#include "ObjectManager.hpp"
#include "Vec2Less.hpp"
#include "TileManager.hpp"
#include "GenerationData.hpp"

class CollisionSystem;



	
class World
{
public:
	World(
		const GenerationData& generation_data,
		const SpriteSheet& tileset,
		const SpriteSheet& object_spritesheet,
		std::shared_ptr<CollisionSystem> collision_system,
		std::shared_ptr<ObjectManager> object_manager,
		int width_tiles, int height_tiles
	);
	~World();

	//Setters
	void setCollisionSystem(std::shared_ptr<CollisionSystem> collision_system);

	void update(Renderer& screen, float dt, const glm::vec2& target);
	void render(Renderer& screen) const;

	void placeTile(int x, int y, BlockType block);
	void damageTile(int x, int y, float damage);

	void updateTiles();

	void generateWorld(std::optional<int> seed);
private:
	void splitGrid(const Grid<Tile>& grid, int chunk_width, int chunk_height);
	
	void initSeeds(std::optional<int> seed_opt);
	GenerationData generation_data;

	std::array<int, 7> seeds;

	void generateBase();
	void addGrass();
	void addDirt();
	void addCaves();
	void addWater();
	void addBiomes();
	void addObjects();
	void applyChanges();

	std::optional<ObjectProperties> getProperties(int id) const;
	const TileProperties& getTileProperties(int id) const;
	//std::map<BlockType, int> tile_presets;

	SDL_FRect camera_rect;

	//Objects
	std::weak_ptr<ObjectManager> object_manager;
	std::weak_ptr<CollisionSystem> collision_system;

	

	//TileManager tile_manager;
	std::vector<Chunk> chunks;

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;

	SpriteSheet tileset;
	SpriteSheet object_spritesheet;

	std::map<glm::ivec2, Object, Vec2Less> objects;
	std::vector<Object> active_objects;

	Grid<Tile> world_map;


	int width_tiles;
	int height_tiles;
};