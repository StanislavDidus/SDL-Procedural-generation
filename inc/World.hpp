#pragma once

#include "glm/glm.hpp"

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

enum class BlockType
{
	GRASS,
	DIRT,
	STONE,

	SNOW_GRASS,
	SNOW_DIRT,

	SAND,

	ROCK,

	SKY,
	WATER,
};

enum class BiomeType
{
	FOREST,
	DESERT,
	TUNDRA,
};
	
class World
{
public:
	World(Renderer& screen, std::shared_ptr<ObjectManager> object_manager, int width_tiles, int height_tiles);
	~World();

	void render();

	//Setters
	void setObjectManager(std::shared_ptr<ObjectManager> object_manager);

	//Getters
	glm::ivec2 getSize() const;

	const Tile& getTile(int x, int y);
	void placeTile(int x, int y, BlockType block);
	void damageTile(int x, int y, float damage);

	const std::vector<Tile>& getTiles() const;
	const std::vector<Chunk>& getChunks() const;

	void updateTiles();

	void resetChunks();

	void clear(); // Clear old chunks from memory
	void generateWorld(std::optional<int> seed);

	float scale = 0.5f;

	float density_change = 0.2f;
	float density_threshold = 0.5f;
	float y_base = 25.f;

	float cave_threshold = 0.1f;
	float cave_threshold_min = 0.15f;
	float cave_threshold_max = 0.5f;
	float cave_threshold_step = 0.002f;
	float cave_base_height = 10.f;
	
	float tunnel_threshold_min = 0.6f;
	float tunnel_threshold_max = 0.8f;

	float sea_level = -5.f;

	std::array<int, 11> seeds;

	NoiseSettings peaks_and_valleys_settings; 
	NoiseSettings density_settings;
	NoiseSettings cave_settings;
	NoiseSettings tunnel_settings;
	NoiseSettings temperature_settings;
	NoiseSettings moisture_settings;

	Biome forest;
	Biome tundra;
	Biome desert;

private:
	void initTiles();
	void initSeeds(std::optional<int> seed);
	void initNoiseSettings();
	void initBiomes();
	void initMaps();

	void generateBase();
	void addDirtGrass();
	void splitWorld();
	/*void addSurface(std::vector<Tile>& tiles);
	void addDirt(std::vector<Tile>& tiles);
	void addWater(std::vector<Tile>& tiles);
	void addCaves(std::vector<Tile>& tiles);
	void addTunnels(std::vector<Tile>& tiles);
	void addBiomes(std::vector<Tile>& tiles);
	void addObjects(std::vector<Tile>& tiles);
	void applyChanges(std::vector<Tile>& tiles);*/

	bool isTileSolid(int x, int y) const; // Generate tile at x, y position and return true if solid

	std::map<glm::ivec2, std::vector<Tile>> changes;
	std::map<BlockType, Tile> tile_presets;

	//Objects
	std::weak_ptr<ObjectManager> object_manager;

	glm::ivec2 getChunkIndex(int x, int y) const;

	Renderer& screen;

	std::map<float, std::vector<float>> surface_map;

	MapRange peaks_and_valleys_map_range_height;
	MapRange peaks_and_valleys_map_range_change;
	MapRange caves_threshold_change;

	std::vector<Chunk> new_chunks;
	std::vector<Chunk> old_chunks;

	Chunk& getOrCreateChunk(int x, int y);

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;

	std::vector<Tile> tiles;
	std::vector<Chunk> chunks;

	int width_tiles;
	int height_tiles;

	int world_begin_x = -500;
	int world_begin_y = -500;
};