#pragma once

#include "glm/vec2.hpp"

#include <memory>
#include <array>
#include <optional>

#include "Biomes.hpp"
#include "ValueNoise.hpp"
#include "PerlynNoise.hpp"
#include "MapRange.hpp"
#include "Chunk.hpp"
#include "ObjectManager.hpp"
#include "TileManager.hpp"
#include "GenerationData.hpp"

class CollisionSystem;

struct TileChange
{
	size_t id;
	glm::ivec2 position;
};

	
class World
{
public:
	World(
		const GenerationData& generation_data,
		const SpriteSheet& tileset,
		const SpriteSheet& object_spritesheet,
		std::shared_ptr<CollisionSystem> collision_system,
		std::shared_ptr<ObjectManager> object_manager,
		int width_tiles, int height_tiles,
		float tile_width_world, float tile_height_world
	);
	~World() = default;

	//Setters
	void setCollisionSystem(std::shared_ptr<CollisionSystem> collision_system);

	void update(const Renderer& screen, float dt, const glm::vec2& target);
	void render(Renderer& screen) const;

	void rebuildChunks();

	void placeTile(int x, int tile_y, size_t tile_id);
	void damageTile(int tile_x, int tile_y, float damage);
	Object* getObjectOnPosition(const glm::vec2& mouse_global_position);
	std::optional<int> damageObject(const glm::vec2& mouse_global_position, float damage); ///< Returns <b>id</b> of a destroyed object, returns <b>std::nullopt</b> if no object was ndestroyed.

	void updateTiles();

	void generateWorld(std::optional<int> seed);
private:
	void splitGrid(const Grid<Tile>& grid, int chunk_width, int chunk_height);
	Chunk& tilePositionToChunk(const glm::ivec2 tile_position);
	glm::ivec2 getTileLocalPosition(const glm::ivec2 tile_position) const;
	
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
	int next_object_id = 0;

	

	//TileManager tile_manager;
	std::vector<Chunk> chunks;

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;

	SpriteSheet tileset;
	SpriteSheet object_spritesheet;

	//std::map<glm::ivec2, Object, Vec2Less> objects;
	std::vector<Object> objects;
	std::vector<Object> active_objects;

	Grid<Tile> world_map;

	std::vector<TileChange> dirt_tiles;

	float tile_width_world;
	float tile_height_world;
	
	float world_width_chunks;
	float world_height_chunks;

	int width_tiles;
	int height_tiles;
};