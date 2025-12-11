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
	//Start seed is always 0
	seeds[0] = 0;

	for (int i = 1; i < seeds.size(); i++)
	{
		seeds[i] = seeds[0] + i;
	}

	initBiomes();
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

	auto it = std::find(new_chunks.begin(), new_chunks.end(), chunk);
	if (it == new_chunks.end())
	{
		new_chunks.push_back(chunk);
	}

	auto tile_local_position = getTileLocalPosition(x, y);

	return chunk.tiles[tile_local_position.y + tile_local_position.x * chunk_height_tiles].index;
}

void World::resetChunks()
{
	old_chunks = new_chunks;
	new_chunks.clear();
}

void World::clear()
{
	old_chunks.clear();
}

void World::generateWorld()
{
	initSeeds();
	initBiomes();
	old_chunks.clear();
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
		0,           // tile id
		2,           // octaves
		2.5f,        // frequency
		3.f,        // ampltude
		seeds[0],    // seed
		1.f,		 // strength
		1.1f		 // height multiplier
	};

	biomes.push_back(forest);

	Biome desert
	{
		"Desert",    // name
		0.8f,        // temperature
		0.15f,        // moissture
		3,          // tile id
		1,           // octaves
		0.5f,      // frequency
		0.9f,       // ampltude
		seeds[1],    // seed
		1.f,		 // strength
		0.7f		 // height multiplier
	};

	biomes.push_back(desert);

	Biome mountain
	{
		"Mountain",    // name
		0.2f,          // temperature
		0.3f,          // moissture
		6,            // tile id
		5,             // octaves
		5.f,          // frequency
		6.f,          // ampltude
		seeds[2],      // seed
		0.8f,		   // strength
		3.f		   // height multiplier
	};

	biomes.push_back(mountain);

	Biome snow
	{
		"Snow",     // name
		0.1f,       // temperature
		0.6f,       // moissture
		5,          // tile id
		2,          // octaves
		2.f,       // frequency
		3.1f,        // ampltude
		seeds[3],   // seed
		1.f,		// strength
		0.9f		// height multiplier
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

		//float x_slope = Noise::fractal1D<ValueNoise>(1, position * scale, 2.f, 3.f, seeds[4]);
		float moisture = Noise::fractal1D<ValueNoise>(1, position * scale, 0.2f, 1.f, seeds[5]);
		float temperature = Noise::fractal1D<ValueNoise>(1, position * scale, 0.25f, 1.f, seeds[6]);
		float dirt = Noise::fractal1D<ValueNoise>(1, position * scale, 2.f, 5.f, seeds[7]);

		

		float max_weight = -1.f;
		int max_weight_index = -1;

		float combined_height = 0.f;
		float combined_weight = 0.f;
		for (int i = 0; i < biomes.size(); i++)
		{
			const auto& biome = biomes[i];

			float weight = biome.weight(temperature, moisture);
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

		//combined_height = std::clamp(combined_height, 0.f, 1.f);

		//std::cout << combined_height << std::endl;

		int surface_y = static_cast<int>(mapRange(combined_height, 0.f, 1.f, -25.f, 25.f));

		int dirt_level = static_cast<int>(std::floor(mapRange(dirt, 0.f, 1.f, 2.f, 5.f)));

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
				tile_id = 4;
			}
			else if(y > surface_y + dirt_level)
			{
				tile_id = 2;
			}
			else
			{
				tile_id = 1;
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
	for (const auto& chunk : old_chunks)
	{
		if (chunk.index_x == x && chunk.index_y == y)
		{
			return chunk;
		}
	}

	Chunk chunk{ x, y, x * chunk_width_tiles, y * chunk_height_tiles };
	fillChunk(chunk);
	auto& ref = old_chunks.emplace_back(std::move(chunk));
	return ref;
}