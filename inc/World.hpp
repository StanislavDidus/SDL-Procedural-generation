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

class World
{
public:
	World(Renderer& screen);
	~World();

	void render();

	const Tile& getTile(int x, int y);

	void resetChunks();

	void clear(); // Clear old chunks from memory
	void generateWorld();

	float scale = 0.5f;

	float density_change = 0.2f;
	float density_threshold = 0.5f;
	float y_base = 0.f;

	float cave_threshold = 0.1f;
	float cave_threshold_min = 0.2f;
	float cave_threshold_max = 0.5f;
	float cave_threshold_step = 0.0021f;
	float cave_base_height = 20.f;
	
	float tunnel_threshold_min = 0.5f;
	float tunnel_threshold_max = 0.575f;

	float sea_level = 11.f;
private:
	void initSeeds();
	void initBiomes();
	void initMaps();

	void generateBase(Chunk& chunk);
	void findSurface(Chunk& chunk);
	void addDirt(Chunk& chunk);
	void addWater(Chunk& chunk);
	void addCaves(Chunk& chunk);
	void addTunnels(Chunk& chunk);
	void addBiomes(Chunk& chunk);

	bool isTileSurface(int x, int y) const;

	glm::ivec2 getChunkIndex(int x, int y) const;

	Renderer& screen;

	//TileMap& tilemap;
	std::vector<Biome> biomes;
	std::array<uint32_t, 11> seeds;

	std::map<float, std::vector<float>> surface_map;

	MapRange peaks_and_valleys_map_range_height;
	MapRange peaks_and_valleys_map_range_change;
	MapRange caves_threshold_change;

	std::vector<Chunk> new_chunks;
	std::vector<Chunk> old_chunks;

	const Chunk& getOrCreateChunk(int x, int y);

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;
};