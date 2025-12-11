#pragma once

#include "glm/glm.hpp"

#include <memory>
#include <array>
#include <set>

#include "Biomes.hpp"
#include "ValueNoise.hpp"
#include "PerlynNoise.hpp"
#include "Chunk.hpp"

class World
{
public:
	World(Renderer& screen);
	~World();

	void render();

	int getTile(int x, int y);

	void resetChunks();

	void clear(); // Clear old chunks from memory
	void generateWorld();

	float scale = 0.01f;
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



	std::vector<Chunk> new_chunks;
	std::vector<Chunk> old_chunks;

	const Chunk& getOrCreateChunk(int x, int y);

	int chunk_width_tiles = 25;
	int chunk_height_tiles = 25;
};