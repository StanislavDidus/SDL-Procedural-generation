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

World::World(Renderer& screen, TileMap& tilemap) : screen(screen), tilemap(tilemap)
{
	generateWorld();
}

World::~World()
{
}

void World::render()
{

}

void World::generateWorld()
{
	initSeeds();
	initBiomes();
	initWorld();
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
		6,          // tile id
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
	const auto& window_size = screen.getWindowSize();
	int screen_y_offset = 200;
	const auto& view_position = screen.getView();
	auto zoom = screen.getZoom();

	tile_width_world = tilemap.getTileSize().x;
	float left_world = view_position.x;
	float right_world = view_position.x + window_size.x / zoom;
	int begin = static_cast<int>(std::floor(left_world / tile_width_world));
	int end = static_cast<int>(std::ceil(right_world / tile_width_world));

	tilemap.clear();
	chunks.clear();

	for (int x = begin; x < end; ++x)
	{
		float position = static_cast<float>(x);

		float x_slope = Noise::fractal1D(noise, 1, position * scale, 2.f, 3.f, seeds[4]);
		float moisture = Noise::fractal1D(noise, 1, position * scale, 0.8f, 1.05f, seeds[5]);
		float temperature = Noise::fractal1D(noise, 1, position * scale, 0.85f, 1.1f, seeds[6]);
		float dirt = Noise::fractal1D(noise, 1, position * scale, 0.45f, 0.8f, seeds[7]);

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
		int tile_id = biomes[max_weight_index].tile_id;

		//Draw offset
		//int x_offset = static_cast<int>(std::floor(mapRange(x_slope, 0.f, 1.f, -3.f, 3.f)));
		int y_offset = static_cast<int>(mapRange(combined_height, 0.f, 1.f, -25.f, 25.f));

		addTile(tile_id, position, y_offset);

		//Fill space 
		int number_dirt_tiles = static_cast<int>(mapRange(dirt, 0.f, 1.f, 2.f, 5.f));

		//Dirt
		for (int i = 1; i < number_dirt_tiles; i++)
		{
			addTile(14, position, y_offset + i);
		}
		
		//Stone
		int i = number_dirt_tiles;
		while (y_offset + i < maximum_y)
		{
			addTile(21, position, y_offset + i);
			++i;
		}

		//Sky
		i = -1;
		while (y_offset + i > minimum_y)
		{
			addTile(6, position, y_offset + i);
			--i;
		}
	}

	tilemap.setChunks(chunks);
}

void World::addTile(int id, float x, float y)
{
	//Get tile local position inside chunk
	int tile_x = (static_cast<int>(x) % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;
	int tile_y = (static_cast<int>(y) % chunk_height_tiles + chunk_height_tiles) % chunk_height_tiles;

	//Get chunk index
	int chunk_index_x = static_cast<int>(std::floor(x / chunk_width_tiles));
	int chunk_index_y = static_cast<int>(std::floor(y / chunk_height_tiles));

	//Add it to the chunk
	auto& chunk = getOrCreateChunk(chunk_index_x, chunk_index_y);
	chunk.addTile(id, tile_y, tile_x);
}

Chunk& World::getOrCreateChunk(int x, int y)
{
	for (auto& chunk : chunks)
	{
		if (chunk.index_x == x && chunk.index_y == y)
		{
			return chunk;
		}
	}

	chunks.emplace_back(x, y, static_cast<float>(x) * chunk_width_tiles, static_cast<float>(y) * chunk_height_tiles);
	return chunks[chunks.size() - 1];
}