#include "World.hpp"

#include <random>

namespace
{
	std::random_device rd;
	std::minstd_rand rng(rd());
}

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

World::World(Renderer& screen) : screen(screen)
{
	generateWorld();
}

World::~World()
{
}

void World::render()
{

}

int World::getTile(int x, int y)
{
	auto chunk_index = getChunkIndex(x, y);

	const auto& chunk = getOrCreateChunk(chunk_index.x, chunk_index.y);

	auto tile_local_position = getTileLocalPosition(x, y);

	for (const auto& tile : chunk.tiles)
	{
		if (tile.row == tile_local_position.y && tile.column == tile_local_position.x)
		{
			return tile.index;
		}
	}

	return 0;
}

void World::generateWorld()
{
	initSeeds();
	initBiomes();
	//initWorld();
	chunks.clear();
}

void World::initSeeds()
{
	std::uniform_int_distribution dist(0, 10000000);
	seeds[0] = dist(rng);

	for (int i = 1; i < seeds.size(); i++)
	{
		seeds[i] = seeds[0] + i;
	}
}

void World::initBiomes()
{
	biomes.clear();
	
	Biome forest
	{
		"Forest",    // name
		0.4f,        // temperature
		0.4f,        // moissture
		8,           // tile id
		2,           // octaves
		0.9f,        // frequency
		0.7f,        // ampltude
		seeds[0],    // seed
		1.f,		 // strength
		1.3f		 // height multiplier
	};

	biomes.push_back(forest);

	Biome desert
	{
		"Desert",    // name
		0.8f,        // temperature
		0.1f,        // moissture
		11,          // tile id
		1,           // octaves
		0.001f,      // frequency
		0.05f,       // ampltude
		seeds[1],    // seed
		0.9f,		 // strength
		0.9f		 // height multiplier
	};

	biomes.push_back(desert);

	Biome mountain
	{
		"Mountain",    // name
		0.2f,          // temperature
		0.4f,          // moissture
		30,            // tile id
		5,             // octaves
		2.1f,          // frequency
		5.1f,          // ampltude
		seeds[2],      // seed
		1.f,		   // strength
		4.5f		   // height multiplier
	};

	biomes.push_back(mountain);

	Biome snow
	{
		"Snow",     // name
		0.1f,       // temperature
		0.6f,       // moissture
		0,          // tile id
		2,          // octaves
		0.8f,       // frequency
		1.f,        // ampltude
		seeds[3],   // seed
		0.9f,		// strength
		0.1f		// height multiplier
	};

	biomes.push_back(snow);
}

void World::initWorld()
{

}

void World::fillChunk(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		float position = static_cast<float>(x);

		float x_slope = Noise::fractal1D(noise, 1, position * scale, 2.f, 3.f, seeds[4]);
		float moisture = Noise::fractal1D(noise, 1, position * scale, 0.8f, 1.05f, seeds[5]);
		float temperature = Noise::fractal1D(noise, 1, position * scale, 0.85f, 1.1f, seeds[6]);
		float dirt = Noise::fractal1D(noise, 1, position * scale, 0.45f, 1.f, seeds[7]);

		float max_weight = -1.f;
		int max_weight_index = -1;

		float combined_height = 0.f;
		float combined_weight = 0.f;
		for (int i = 0; i < biomes.size(); i++)
		{
			const auto& biome = biomes[i];

			float weight = biome.weight(temperature, moisture) * biome.strength;
			float height = biome.height<ValueNoise>(position * scale);

			combined_height += height * weight;
			combined_weight += weight;

			if (weight > max_weight)
			{
				max_weight = weight;
				max_weight_index = i;
			}
		}
		combined_height /= combined_weight;

		int surface_y = static_cast<int>(mapRange(combined_height, 0.f, 1.f, -25.f, 25.f));

		int dirt_level = static_cast<int>(std::floor(mapRange(dirt, 0.f, 1.f, 2.f, 4.f)));

		int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;

		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			int tile_id = 0;

			if (y == surface_y)
			{
				tile_id = biomes[max_weight_index].tile_id;
			}
			else if (y < surface_y)
			{
				tile_id = 6;
			}
			else if(y > surface_y + dirt_level)
			{
				tile_id = 21;
			}
			else
			{
				tile_id = 14;
			}

			int tile_y = (y % chunk_height_tiles + chunk_height_tiles) % chunk_height_tiles;

			chunk.addTile(tile_id, tile_y, tile_x);
		}
	}
}

glm::ivec2 World::getChunkIndex(int x, int y) const
{
	int chunk_index_x = static_cast<int>(std::floor(static_cast<float>(x) / static_cast<float>(chunk_width_tiles)));
	int chunk_index_y = static_cast<int>(std::floor(static_cast<float>(y) / static_cast<float>(chunk_height_tiles)));

	return glm::ivec2{chunk_index_x, chunk_index_y};
}

glm::ivec2 World::getTileLocalPosition(int x, int y) const
{
	int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;
	int tile_y = (y % chunk_height_tiles + chunk_height_tiles) % chunk_height_tiles;

	return glm::ivec2{tile_x, tile_y};
}

const Chunk& World::getOrCreateChunk(int x, int y)
{
	for (const auto& chunk : chunks)
	{
		if (chunk.index_x == x && chunk.index_y == y)
		{
			return chunk;
		}
	}

	Chunk chunk{ x, y, x * chunk_width_tiles, y * chunk_height_tiles };
	fillChunk(chunk);
	chunks.emplace_back(std::move(chunk));
	return chunks[chunks.size() - 1];
}