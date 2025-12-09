#pragma once

#include <memory>
#include <array>

#include "glm/glm.hpp"

#include "Biomes.hpp"
#include "ValueNoise.hpp"
#include "Chunk.hpp"

class World
{
public:
	World(Renderer& screen);
	~World();

	void render();

	int getTile(int x, int y);

	void generateWorld();
private:
	void initSeeds();
	void initBiomes();
	void initWorld();

	void fillChunk(Chunk& chunk);

	glm::ivec2 getChunkIndex(int x, int y) const;
	glm::ivec2 getTileLocalPosition(int x, int y) const;

	Renderer& screen;

	//TileMap& tilemap;
	std::vector<Biome> biomes;
	std::array<uint32_t, 8> seeds;

	ValueNoise noise;

	float scale = 0.01f;

	std::vector<Chunk> chunks;

	const Chunk& getOrCreateChunk(int x, int y);

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;
};