#include "World.hpp"

#include <random>


namespace
{
	std::random_device rd;
	std::minstd_rand rng(rd());
}

static int mapRange(int x, int inMin, float inMax, int outMin, int outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}

World::World(Renderer& screen)
	: screen(screen)
	, peaks_and_valleys_map_range_height(0.f, 1.f, 25.f, -20.f)
	, peaks_and_valleys_map_range_change(0.f, 1.f, 0.9f, 0.15f)
	, caves_threshold_change(0.f, 300.f, cave_threshold_min, cave_threshold_max)
{
	//Start seed is always 0
	seeds[0] = 0;

	for (int i = 1; i < seeds.size(); i++)
	{
		seeds[i] = seeds[0] + i;
	}

	initBiomes();
	initMaps();
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

	glm::vec2 tile_local_position = { x - chunk.x, y - chunk.y };

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
	surface_map.clear();
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
		0.f,		 // pv min
		1.f,        // pv max
		0.4f,        //temperature min
		0.8f,         // temperature max
		0.f,         // moisture min
		1.f,		 // moiseture max
		0,           // tile id
	};

	biomes.push_back(forest);

	Biome desert
	{
		"Desert",    // name
		0.f,		 // pv min
		0.6f,        // pv max
		0.8f,        //temperature min
		1.f,         // temperature max
		0.f,         // moisture min
		1.f,		 // moiseture max
		3,           // tile id
	};

	biomes.push_back(desert);

	//Biome mountain
	//{
	//	"Mountain",    // name
	//	0.2f,          // temperature
	//	0.3f,          // moissture
	//	6,            // tile id
	//	5,             // octaves
	//	5.f,          // frequency
	//	6.f,          // ampltude
	//	seeds[2],      // seed
	//	0.8f,		   // strength
	//	3.f		   // height multiplier
	//};

	//biomes.push_back(mountain);

	Biome snow
	{
		"Snow",    // name
		0.f,		 // pv min
		1.f,        // pv max
		0.0f,        //temperature min
		0.4f,         // temperature max
		0.f,         // moisture min
		1.f,		 // moiseture max
		4,           // tile id
	};

	biomes.push_back(snow);
}

void World::initMaps()
{
	peaks_and_valleys_map_range_height.addPoint(0.2f, 17.f);
	peaks_and_valleys_map_range_height.addPoint(0.5f, 4.f);
	peaks_and_valleys_map_range_height.addPoint(0.75f, -8.f);
	peaks_and_valleys_map_range_height.addPoint(0.8f, -19.f);

	peaks_and_valleys_map_range_change.addPoint(0.2f, 0.75f);
	peaks_and_valleys_map_range_change.addPoint(0.5f, 0.4f);
	peaks_and_valleys_map_range_change.addPoint(0.75f, 0.12f);

	caves_threshold_change.addPoint(150.f, 0.25f);
}

void World::generateBase(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;

		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(3, position_x * scale, 0.085f, 0.8f, seeds[4]);

		float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

		float map_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);
		float correlated_change = map_change;

		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			float density_noise = Noise::fractal2D<ValueNoise>(8, position_x * scale, position_y * scale, 0.75f, 1.25f, seeds[5]);

			float correlated_height = position_y - map_height;

			//Apply y base layer and change over vertical axis
			float difference = y_base - correlated_height;
			float change = difference * correlated_change;
			density_noise += change;

			int tile_id = 0;
			TileType type = TileType::NONE;

			if (density_noise < density_threshold)
			{
				tile_id = 2;
				type = TileType::SOLID;
			}
			if (density_noise >= density_threshold)
			{
				//Air tile
				tile_id = 4;
				type = TileType::AIR;
			}

			int tile_y = (y % chunk_height_tiles + chunk_height_tiles) % chunk_height_tiles;

			chunk.addTile(tile_id, tile_y, tile_x, type);
		}
	}
}

void World::findSurface(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		bool is_air = false;
		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			if (isTileSurface(x, y))
			{
				tile.type = TileType::SURFACE;
				tile.index = 1;
			}

			//Check if tiles is on the top of the chunk

			//if (tile_local_y == 0 && tile.type == TileType::SOLID)
			//{
			//	//Find position of the tile above
			//	float position_y_top = position_y - 1.f;
			//	float position_x_top = position_x;

			//	//Find pv
			//	float peaks_and_valleys = Noise::fractal1D<ValueNoise>(3, position_x_top * scale, 0.085f, 0.8f, seeds[4]);

			//	float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

			//	float map_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);
			//	float correlated_change = map_change;

			//	//Find density
			//	float density_noise = Noise::fractal2D<ValueNoise>(8, position_x_top * scale, position_y_top * scale, 0.75f, 1.25f, seeds[5]);

			//	float correlated_height = position_y_top - map_height;

			//	//Apply y base layer and change over vertical axis
			//	float difference = y_base - correlated_height;
			//	float change = difference * correlated_change;
			//	density_noise += change;

			//	if (density_noise >= density_threshold)
			//	{
			//		//Tile above is air
			//		tile.type = TileType::SURFACE;
			//		tile.index = 1;
			//		surface_map[position_x].push_back(position_y);
			//	}
			//}
			//else
			//{
			//	if (tile.type == TileType::AIR)
			//	{
			//		is_air = true;
			//	}
			//	else if (tile.type == TileType::SOLID && is_air)
			//	{
			//		tile.type = TileType::SURFACE;
			//		tile.index = 1;
			//		is_air = false;
			//		surface_map[position_x].push_back(position_y);
			//	}
			//	else
			//	{
			//		is_air = false;
			//	}
			//}
		}
	}
}

void World::addDirt(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);

		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			if (tile.type != TileType::SOLID) continue;

			int dirt_level = 10;

			for (int i = 0; i < dirt_level; ++i)
			{
				if (isTileSurface(x, y - i))
				{
					tile.index = 1;
					break;
				}
			}
		}
	}

	/*for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);

		float dirt_noise = Noise::fractal1D<ValueNoise>(3, position_x * scale, 0.2f, 0.5f, seeds[10]);
		float dirt_level = dirt_noise * 15.f;

		bool surface_found = false;
		int surface_position_y = 0;

		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			if (tile.type == TileType::SOLID)
			{
				float max = std::numeric_limits<float>::lowest();
				for (const auto& surface_y : surface_map[x])
				{
					if (surface_y >= position_y) continue;

					float difference = surface_y - position_y;
					if (difference > max) max = difference;
				}

				float dirt_to_spawn = max + 1.f + dirt_level;

				if (dirt_to_spawn > 0.f)
				{
					tile.index = 1;
				}
			}
		}
	}*/
}

void World::addWater(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			if (tile.type == TileType::AIR && position_y > sea_level && !tile.sealed)
			{
				tile.type = TileType::WATER;
				tile.index = 7;
			}
		}
	}
}

void World::addCaves(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			//Cave noise
			float cave_noise = Noise::fractal2D<ValueNoise>(8, position_x * scale, position_y * scale, 0.1f, 0.75f, seeds[6]);

			float c_difference = (cave_base_height - position_y) * -1.f;
			float correlated_cave_threshold = std::clamp(caves_threshold_change.getValue(c_difference), 0.001f, cave_threshold_max);

			if (tile.type == TileType::SOLID && cave_noise < correlated_cave_threshold)
			{
				tile.type = TileType::AIR;
				tile.index = 4;
				tile.sealed = true;
			}
		}
	}
}

void World::addTunnels(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			float tunnel_noise = Noise::fractal2D<ValueNoise>(8, position_x * scale, position_y * scale, 0.1f, 1.2f, seeds[7]);

			if (tile.type == TileType::SOLID)
			{
				if (tunnel_noise > tunnel_threshold_min && tunnel_noise < tunnel_threshold_max)
				{
					tile.type = TileType::AIR;
					tile.index = 4;
					tile.sealed = true;
				}
			}
		}
	}
}

void World::addBiomes(Chunk& chunk)
{ 
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			int tile_local_x = x - chunk.x;
			int tile_local_y = y - chunk.y;

			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

			if (tile.type != TileType::SOLID) continue;

			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(3, position_x * scale, 0.085f, 0.8f, seeds[4]);
			float temperature = Noise::fractal2D<ValueNoise>(3, position_x * scale, position_y * scale, 0.01f, 0.7f, seeds[8]);
			float moisture = Noise::fractal2D<ValueNoise>(3, position_x * scale, position_y * scale, 0.015f, 0.6f, seeds[9]);

			int tile_id = 0;

			if (tile.index != 1) continue;

			if (peaks_and_valleys < 0.6f)
			{
				//Desert,Forest,Snow
				if (temperature < 0.4f && moisture > 0.6f)
				{
					//Snow
					tile_id = 5;
				}
				else if (temperature >= 0.7f && moisture > 0.6f)
				{
					//Desert
					tile_id = 3;
				}
				else
				{
					//Forest
					tile_id = 0;
				}
			}
			else if (peaks_and_valleys < 1.f)
			{
				//Forest,Snow
				if (temperature < 0.5f && moisture > 0.5f)
				{
					//Snow
					tile_id = 5;
				}
				else
				{
					//Forest
					tile_id = 0;
				}
			}

			tile.index = tile_id;
		}
	}
}

bool World::isTileSurface(int x, int y) const
{
	//Check if tile at (x,y) is solide
	//If is solid then check if tile above it is air
	//If is air than tile is surface
	{
		float position_x = static_cast<float>(x);
		float position_y = static_cast<float>(y);

		//Find pv
		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(3, position_x * scale, 0.085f, 0.8f, seeds[4]);

		float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

		float map_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);
		float correlated_change = map_change;

		//Find density
		float density_noise = Noise::fractal2D<ValueNoise>(8, position_x * scale, position_y * scale, 0.75f, 1.25f, seeds[5]);

		float correlated_height = position_y - map_height;

		//Apply y base layer and change over vertical axis
		float difference = y_base - correlated_height;
		float change = difference * correlated_change;
		density_noise += change;

		//Tile at (x,y) is air - skip
		if (density_noise >= density_threshold) return false;
	}

	{
		float position_x_top = static_cast<float>(x);
		float position_y_top = static_cast<float>(y - 1);

		//Find pv
		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(3, position_x_top * scale, 0.085f, 0.8f, seeds[4]);

		float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

		float map_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);
		float correlated_change = map_change;

		//Find density
		float density_noise = Noise::fractal2D<ValueNoise>(8, position_x_top * scale, position_y_top * scale, 0.75f, 1.25f, seeds[5]);

		float correlated_height = position_y_top - map_height;

		//Apply y base layer and change over vertical axis
		float difference = y_base - correlated_height;
		float change = difference * correlated_change;
		density_noise += change;

		if (density_noise >= density_threshold)
		{
			//Tile is a surface
			return true;
		}
	}

	return false;
}

glm::ivec2 World::getChunkIndex(int x, int y) const
{
	int chunk_index_x = static_cast<int>(std::floor(static_cast<float>(x) / static_cast<float>(chunk_width_tiles)));
	int chunk_index_y = static_cast<int>(std::floor(static_cast<float>(y) / static_cast<float>(chunk_height_tiles)));

	return glm::ivec2{ chunk_index_x, chunk_index_y };
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

	generateBase(chunk);
	//findSurface(chunk);
	addDirt(chunk);
	addCaves(chunk);
	//addTunnels(chunk);
	addWater(chunk);
	//addBiomes(chunk);

	auto& ref = old_chunks.emplace_back(std::move(chunk));
	return ref;
}