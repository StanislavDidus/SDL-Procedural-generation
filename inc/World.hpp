#pragma once

#include "glm/vec2.hpp"

#include <memory>
#include <array>
#include <optional>
#include <UI/Button.hpp>
#include <UI/Button.hpp>
#include <UI/Button.hpp>
#include <UI/Button.hpp>

#include "Biomes.hpp"
#include "ValueNoise.hpp"
#include "PerlynNoise.hpp"
#include "MapRange.hpp"
#include "Chunk.hpp"
#include "ObjectManager.hpp"
#include "TileManager.hpp"
#include "GenerationData.hpp"
#include "RenderFunctions.hpp"

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
		std::shared_ptr<CollisionSystem> collision_system,
		int width_tiles, int height_tiles,
		float tile_width_world, float tile_height_world
	);
	~World() = default;

	//Getters
	const Grid<Tile>& getGrid() const;
	glm::ivec2 getWorldSizeTiles() const; ///< Returns world's size in tiles.
	glm::vec2 getTileSize() const;

	//Setters
	void setCollisionSystem(std::shared_ptr<CollisionSystem> collision_system);

	void update(const graphics::Renderer& screen, float dt, const glm::vec2& target);
	void render(graphics::Renderer& screen) const;


	void placeTile(int x, int tile_y, size_t tile_id);
	void damageTile(int tile_x, int tile_y, float damage);
	Object* getObjectOnPosition(const glm::vec2& mouse_global_position);
	std::optional<int> damageObject(const glm::vec2& mouse_global_position, float damage); ///< Returns <b>id</b> of a destroyed object, returns <b>std::nullopt</b> if no object was ndestroyed.

	void updateTiles();

	void generateWorld(std::optional<int> seed);
private:
	void splitGrid(const Grid<Tile>& grid, const std::vector<Object>& objects, int chunk_width, int chunk_height);
	Chunk& tilePositionToChunk(const glm::ivec2 tile_position);
	glm::ivec2 getTileLocalPosition(const glm::ivec2 tile_position) const;
	
	void initSeeds(std::optional<int> seed_opt);
	uint32_t getNewSeed(uint32_t master_seed);
	GenerationData generation_data;

	std::array<int, 7> seeds;

	void generateBase();
	void addGrass();
	void addDirt();
	void addCaves();
	void addWater();
	void addBiomes();
	void addObjects(std::vector<Object>& objects);

	std::optional<ObjectProperties> getProperties(int id) const;
	const TileProperties& getTileProperties(int id) const;
	//std::map<BlockType, int> tile_presets;

	SDL_FRect camera_rect;

	//Objects
	std::weak_ptr<CollisionSystem> collision_system;
	int next_object_id = 0;

	size_t seeds_count = 0;

	//TileManager tile_manager;
	std::vector<Chunk> chunks;

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;

	Grid<Tile> world_map;

	std::vector<TileChange> dirt_tiles;

	float tile_width_world;
	float tile_height_world;
	
	float world_width_chunks;
	float world_height_chunks;

	int world_width_tiles;
	int world_height_tiles;
};