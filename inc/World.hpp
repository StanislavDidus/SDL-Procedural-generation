#pragma once

#include <memory>
#include <array>

#include "TileMap.hpp"
#include "Biomes.hpp"
#include "ValueNoise.hpp"

class World
{
public:
	World(Renderer& screen, TileMap& tilemap);
	~World();

	void render();

	void generateWorld();
private:
	void initSeeds();
	void initBiomes();
	void initWorld();

	Renderer& screen;

	TileMap& tilemap;
	std::vector<Biome> biomes;
	std::array<uint32_t, 7> seeds;

	ValueNoise noise;

	float scale = 0.01f;

	std::vector<Chunk> chunks;

	Chunk& getOrCreateChunk(int index);

	float tile_width_world = 0.f;

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 100;
};