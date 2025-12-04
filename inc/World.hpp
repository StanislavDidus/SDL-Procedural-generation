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

	void addTile(int id, float x, float y); //Take global tile grid position and add tile to corresponding chunk

	Renderer& screen;

	TileMap& tilemap;
	std::vector<Biome> biomes;
	std::array<uint32_t, 8> seeds;

	ValueNoise noise;

	float scale = 0.01f;

	std::vector<Chunk> chunks;

	Chunk& getOrCreateChunk(int x, int y);

	float tile_width_world = 0.f;

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;

	int minimum_y = -15;
	int maximum_y = 50;
};