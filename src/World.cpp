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
	, peaks_and_valleys_map_range_height(0.4f, 1.8f, 0.f, -25.f)
	, peaks_and_valleys_map_range_change(0.4f, 1.8f, 0.9f, 0.15f)
	, caves_threshold_change(y_base, y_base + 300.f, cave_threshold_min, cave_threshold_max)
{
	//Start seed is always 0
	seeds[0] = 0;

	for (int i = 1; i < seeds.size(); i++)
	{
		seeds[i] = seeds[0] + i;
	}

	initNoiseSettings();
	initBiomes();
	initMaps();
}

World::~World()
{
}

void World::render()
{

}

const Tile& World::getTile(int x, int y)
{
	auto chunk_index = getChunkIndex(x, y);

	const auto& chunk = getOrCreateChunk(chunk_index.x, chunk_index.y);

	auto it = std::find(new_chunks.begin(), new_chunks.end(), chunk);
	if (it == new_chunks.end())
	{
		new_chunks.push_back(chunk);
	}

	glm::vec2 tile_local_position = { x - chunk.x, y - chunk.y };

	return chunk.tiles[tile_local_position.y + tile_local_position.x * chunk_height_tiles];
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

void World::generateWorld(std::optional<int> seed)
{
	initSeeds(seed);
	//initNoiseSettings();

	peaks_and_valleys_settings.seed = seeds[4];
	density_settings.seed = seeds[5];
	cave_settings.seed = seeds[6];
	tunnel_settings.seed = seeds[7];
	temperature_settings.seed = seeds[8];
	moisture_settings.seed = seeds[9];

	initBiomes();
	old_chunks.clear();
	surface_map.clear();
}

void World::initSeeds(std::optional<int> seed)
{
	if (!seed.has_value())
	{
		std::uniform_int_distribution dist(0, 10000000);
		seeds[0] = dist(rng);
	}
	else
	{
		seeds[0] = seed.value();
	}

	for (int i = 1; i < seeds.size(); i++)
	{
		seeds[i] = seeds[0] + i;
	}
}

void World::initNoiseSettings()
{
	peaks_and_valleys_settings.octaves = 5;
	peaks_and_valleys_settings.frequency = 0.05f;
	peaks_and_valleys_settings.amplitude = 1.0f;
	peaks_and_valleys_settings.seed = seeds[4];

	density_settings.octaves = 8;
	density_settings.frequency = 0.75f;
	density_settings.amplitude = 1.0f;
	density_settings.seed = seeds[5];

	cave_settings.octaves = 8;
	cave_settings.frequency = 0.1f;
	cave_settings.amplitude = 1.0f;
	cave_settings.seed = seeds[6];

	tunnel_settings.octaves = 8;
	tunnel_settings.frequency = 0.1f;
	tunnel_settings.amplitude = 1.0f;
	tunnel_settings.seed = seeds[7];

	temperature_settings.octaves = 1;
	temperature_settings.frequency = 0.05f;
	temperature_settings.amplitude = 1.0f;
	temperature_settings.seed = seeds[8];

	moisture_settings.octaves = 2;
	moisture_settings.frequency = 0.04f;
	moisture_settings.amplitude = 1.2f;
	moisture_settings.seed = seeds[9];
}

void World::initBiomes()
{
	forest.name = "Forest";
	forest.pv_min = 0.0f;
	forest.pv_max = 1.0f;
	forest.temperature_min = 0.4f;
	forest.temperature_max = 0.8f;
	forest.moisture_min = 0.f;
	forest.moisture_max = 1.0f;
	forest.tile_surface_id = 0;
	forest.tile_dirt_id = 1;

	tundra.name = "Tundra";
	tundra.pv_min = 0.f;
	tundra.pv_max = 1.f;
	tundra.temperature_min = 0.0f;
	tundra.temperature_max = 0.4f;
	tundra.moisture_min = 0.f;
	tundra.moisture_max = 1.f;
	tundra.tile_surface_id = 5;
	tundra.tile_dirt_id = 1;

	desert.name = "Desert";
	desert.pv_min = 0.0f;
	desert.pv_max = 0.6f;
	desert.temperature_min = 0.8f;
	desert.temperature_max = 1.0f;
	desert.moisture_min = 0.0f;
	desert.moisture_max = 1.0f;
	desert.tile_surface_id = 3;
	desert.tile_dirt_id = 3;
}

void World::initMaps()
{
	/*peaks_and_valleys_map_range_height.addPoint(0.2f, 17.f);
	peaks_and_valleys_map_range_height.addPoint(0.5f, 4.f);
	peaks_and_valleys_map_range_height.addPoint(0.6f, -8.f);
	peaks_and_valleys_map_range_height.addPoint(0.85f, -19.f);*/

	//peaks_and_valleys_map_range_height.addPoint(1.5f, -5.f);
	//peaks_and_valleys_map_range_height.addPoint(1.8f, -5.f);

	//peaks_and_valleys_map_range_height.addPoint(0.75f, -10.f);
	//peaks_and_valleys_map_range_height.addPoint(1.2f, -11.f);
	//peaks_and_valleys_map_range_height.addPoint(0.9f, -13.f);
	//peaks_and_valleys_map_range_height.addPoint(1.5f, -15.f);
	//peaks_and_valleys_map_range_height.addPoint(0.75f, -15.f);

	//peaks_and_valleys_map_range_change.addPoint(0.5f, 0.9f);
	//peaks_and_valleys_map_range_change.addPoint(0.65f, 0.8f);
	//peaks_and_valleys_map_range_change.addPoint(0.9f, 0.2f);

	caves_threshold_change.addPoint(y_base + 25.f, 0.25f);
}

void World::generateBase(Chunk& chunk)
{
	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;

		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(peaks_and_valleys_settings, position_x * scale);

		float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

		float new_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);

		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			float density_noise = Noise::fractal2D<ValueNoise>(density_settings, position_x * scale, position_y * scale);

			float new_height = position_y - map_height;

			//Apply y base layer and change over vertical axis
			float difference = new_height - y_base;
			float change = difference * new_change;
			density_noise += change;

			int tile_id = 0;
			TileType type = TileType::NONE;
			bool solid = false;

			if (density_noise >= density_threshold)
			{
				//Solid tile
				tile_id = 2;
				type = TileType::STONE;
				solid = true;
			}
			if (density_noise < density_threshold)
			{
				//Air tile
				tile_id = 4;
				//type = TileType::AIR;
				solid = false;
			}

			int tile_y = (y % chunk_height_tiles + chunk_height_tiles) % chunk_height_tiles;

			Tile tile{ tile_id, tile_x, tile_y, type };
			tile.solid = solid;
			chunk.addTile(tile);
		}
	}
}

void World::addSurface(Chunk& chunk)
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

			if (!tile.solid) continue;

			int top_local_y = tile_local_y - 1;

			if (top_local_y >= 0)
			{
				auto& tile1 = chunk.tiles[top_local_y + tile_local_x * chunk_height_tiles];

				if (!tile1.solid)
				{
					tile.index = 0;
					tile.type = TileType::SURFACE;
				}
			}
			else
			{
				if (!isTileSolid(x, y - 1))
				{
					tile.index = 0;
					tile.type = TileType::SURFACE;
				}
			}
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

			if (!tile.solid) continue;

			float dirt_noise = ValueNoise::noise1D(position_x * scale * 0.2f, seeds[2]) * 1.f;

			int dirt_level = static_cast<int>(dirt_noise * 5.f) + 5;

			for (int i = 0; i < dirt_level; ++i)
			{
				int new_local_y = tile_local_y - i;
				int new_y = y - i;

				if (new_local_y >= 1)
				{
					auto& tile1 = chunk.tiles[new_local_y + tile_local_x * chunk_height_tiles];
					auto& tile2 = chunk.tiles[new_local_y - 1 + tile_local_x * chunk_height_tiles];

					if (tile1.solid && !tile2.solid)
					{
						tile.type = TileType::DIRT;
						tile.index = 1;
						break;
					}
				}
				else
				{
					if (isTileSolid(x, new_y) && !isTileSolid(x, new_y - 1))
					{
						tile.type = TileType::DIRT;
						tile.index = 1;
						break;
					}
				}
			}
		}
	}
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

			if (!tile.solid && position_y > sea_level && !tile.sealed)
			{
				tile.type = TileType::WATER;
				//tile.solid = false;
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
			float cave_noise = Noise::fractal2D<ValueNoise>(cave_settings, position_x * scale, position_y * scale);

			float c_difference = (cave_base_height - position_y) * -1.f;
			float correlated_cave_threshold = std::clamp(caves_threshold_change.getValue(c_difference), 0.001f, cave_threshold_max);

			if (tile.solid && cave_noise < correlated_cave_threshold)
			{
				tile.type = TileType::NONE;
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

			float tunnel_noise = Noise::fractal2D<ValueNoise>(tunnel_settings, position_x * scale, position_y * scale);

			if (tile.solid)
			{
				if (tunnel_noise > tunnel_threshold_min && tunnel_noise < tunnel_threshold_max)
				{
					tile.type = TileType::NONE;
					tile.solid = false;
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

			if (tile.type != TileType::SURFACE && tile.type != TileType::DIRT) continue;

			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(peaks_and_valleys_settings, position_x * scale);
			float temperature = Noise::fractal2D<ValueNoise>(temperature_settings, position_x * scale, position_y * scale);
			float moisture = Noise::fractal2D<ValueNoise>(moisture_settings, position_x * scale, position_y * scale);

			int tile_id = 0;

			

			//Desert,Forest,Snow
			if (temperature < 0.45f && moisture > 0.55f)
			{
				//Snow
				if (tile.type == TileType::SURFACE)
					tile_id = 5;
				else
					tile_id = 8;
			}
			else if (temperature >= 0.6f && moisture < 0.5f)
			{
				//Desert
				tile_id = 3;
			}
			else
			{
				//Forest
				if (tile.type == TileType::SURFACE)
					tile_id = 0;
				else
					tile_id = 1;
			}

			tile.index = tile_id;

			tile.debug_info.pv = std::min(peaks_and_valleys, 1.f);
			tile.debug_info.temperature = std::min(temperature, 1.f);
			tile.debug_info.moisture = std::min(moisture, 1.f);
		}
	}
}

bool World::isTileSolid(int x, int y) const
{
	//Convert to float
	float position_x = static_cast<float>(x);
	float position_y = static_cast<float>(y);

	//Find pv
	float peaks_and_valleys = Noise::fractal1D<ValueNoise>(peaks_and_valleys_settings, position_x * scale);

	float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

	float map_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);
	float correlated_change = map_change;

	//Find density
	float density_noise = Noise::fractal2D<ValueNoise>(density_settings, position_x * scale, position_y * scale);

	float correlated_height = position_y - map_height;

	//Apply y base layer and change over vertical axis
	float difference = correlated_height - y_base;
	float change = difference * correlated_change;
	density_noise += change;

	return density_noise > density_threshold;
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
	addDirt(chunk);
	addSurface(chunk);
	addCaves(chunk);
	//addTunnels(chunk);
	addWater(chunk);
	addBiomes(chunk);

	auto& ref = old_chunks.emplace_back(std::move(chunk));
	return ref;
}