#include "World.hpp"
#include "Debug.hpp"

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


World::World(const SpriteSheet& tileset, std::shared_ptr<ObjectManager> object_manager, int width_tiles, int height_tiles)
	: tileset(tileset)
	, world_map(tileset, height_tiles, width_tiles)
	, width_tiles(width_tiles)
	, height_tiles(height_tiles)
	, object_manager(object_manager)
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

	initTiles();
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

void World::setObjectManager(std::shared_ptr<ObjectManager> object_manager)
{
	this->object_manager = object_manager;
}

void World::placeTile(int x, int y, BlockType block)
{
	int chunk_x = static_cast<int>(std::floor(x / static_cast<float>(chunk_width_tiles)));
	int chunk_y = static_cast<int>(std::floor(y / static_cast<float>(chunk_height_tiles)));
	
	int height_chunks = height_tiles / chunk_height_tiles;
	int width_chunks = width_tiles / chunk_width_tiles;
	
	if (chunk_x < 0 || chunk_x >= width_chunks ||
		chunk_y < 0 || chunk_y >= height_chunks) return;

	int chunk_index = chunk_y + chunk_x * height_chunks;
	auto& chunk = chunks[chunk_index];

	int tile_local_x = x % chunk_width_tiles;
	int tile_local_y = y % chunk_height_tiles;

	auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

	if (!tile.solid)
	{
		bool is_block = false;
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				if (i == 0 && j == 0) continue;

				float new_x = static_cast<float>(x + i);
				float new_y = static_cast<float>(y + j);
				int chunk_x_index = static_cast<int>(std::floor(new_x / static_cast<float>(chunk_width_tiles)));
				int chunk_y_index = static_cast<int>(std::floor(new_y / static_cast<float>(chunk_height_tiles)));

				if (chunk_x_index < 0 || chunk_x_index >= width_chunks ||
					chunk_y_index < 0 || chunk_y_index >= height_chunks) continue;

				int chunk_index_ = chunk_y_index + chunk_x_index * height_chunks;
				auto& chunk_ = chunks[chunk_index_];

				int chunk_x_grid = chunk_x_index * chunk_width_tiles;
				int chunk_y_grid = chunk_y_index * chunk_height_tiles;

				glm::vec2 tile_local_position_ = glm::vec2{ new_x - chunk_x_grid, new_y - chunk_y_grid };
				auto& tile_ = chunk_.tiles[tile_local_position_.y + tile_local_position_.x * chunk_height_tiles];
				if (tile_.solid)
				{
					is_block = true;
					break;
				}
			}
		}

		if (is_block)
		{
			tile.setTile(tile_presets[block]);
			//changes[chunk_index].emplace_back(tile.sprite_index, static_cast<int>(tile_local_position.x), static_cast<int>(tile_local_position.y), tile.type, tile.solid, tile.max_durability);
		}
	}
}

void World::damageTile(int x, int y, float damage)
{
	int chunk_x = static_cast<float>(std::floor(static_cast<float>(x) / static_cast<float>(chunk_width_tiles)));
	int chunk_y = static_cast<float>(std::floor(static_cast<float>(y) / static_cast<float>(chunk_height_tiles)));

	int height_chunks = height_tiles / chunk_height_tiles;
	int width_chunks = width_tiles / chunk_width_tiles;

	if (chunk_x < 0 || chunk_x >= width_chunks ||
		chunk_y < 0 || chunk_y >= height_chunks) return;

	int chunk_index = chunk_y + chunk_x * height_chunks;
	auto& chunk = chunks[chunk_index];

	int tile_local_x = x % chunk_width_tiles;
	int tile_local_y = y % chunk_height_tiles;

	auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];

	if (tile.solid)
	{
		tile.dealDamage(damage);

		if (tile.is_destroyed)
		{
			tile.setTile(tile_presets[BlockType::SKY]);
		}
	}

	//auto chunk_index = getChunkIndex(x, y);

	//auto& chunk = getOrCreateChunk(chunk_index.x, chunk_index.y);

	//glm::vec2 tile_local_position = { x - chunk.x, y - chunk.y };

	//auto& tile = chunk.tiles[tile_local_position.y + tile_local_position.x * chunk_height_tiles];

	//if (!tile.solid) return;
	//
	//tile.dealDamage(damage);

	////Destroy
	//if (tile.is_destroyed)
	//{
	//	tile.sprite_index = 4;
	//	tile.solid = false;

	//	tile.setTile(tile_presets[BlockType::SKY]);

	//	changes[chunk_index].emplace_back(4, static_cast<int>(tile_local_position.x), static_cast<int>(tile_local_position.y), TileType::NONE, false, 0.f);
	//}
}

const std::vector<Tile>& World::getTiles() const
{
	return tiles;
}

const std::vector<Chunk>& World::getChunks() const
{
	return chunks;
}

glm::ivec2 World::getSize() const
{
	return glm::ivec2{width_tiles, height_tiles};
}

std::optional<ObjectProperties> World::getProperties(int id) const
{
	if (auto s = object_manager.lock())
	{
		return s->getProperties(id);
	}
	return std::nullopt;
}

void World::updateTiles()
{
	for (auto& chunk : chunks)
	{
		for (auto& tile : chunk.tiles)
		{
			if (tile.current_durability < tile.max_durability && !tile.received_damage_last_frame)
			{
				tile.current_durability = tile.max_durability;
			}

			tile.received_damage_last_frame = false;
		}
	}
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
	tiles.clear();
	chunks.clear();
	objects.clear();

	tiles.resize(width_tiles * height_tiles);
	chunks.resize(width_tiles / chunk_width_tiles * height_tiles / chunk_height_tiles);

	generateBase();
	addGrass();
	addDirt();
	addCaves();
	addWater();
	addBiomes();
	addObjects();
	splitWorld();
	//std::cout << chunks.size() << std::endl;
}

void World::splitTileMap(const TileMap& tilemap, int chunk_width, int chunk_height)
{
	chunks.clear();

	int width = tilemap.getColumns();
	int height = tilemap.getRows();

	int world_width_chunks = width / chunk_width;
	int world_height_chunks = height / chunk_height;

	chunks.reserve(world_width_chunks * world_height_chunks);

	for (int i = 0; i < world_width_chunks * world_height_chunks; ++i)
	{
		chunks.emplace_back(tilemap.getSpriteSheet(), SDL_FRect{}, chunk_height, chunk_width);
	}

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int tile_local_x = x % chunk_width;
			int tile_local_y = y % chunk_height;

			int chunk_x = x / chunk_width;
			int chunk_y = y / chunk_height;

			auto& chunk = chunks[chunk_y + chunk_x * world_height_chunks];
			chunk.tilemap(tile_local_x, tile_local_y) = tilemap(x, y);
		}
	}

	float tile_size = 20.f;

	for (int i = 0; auto& chunk : chunks)
	{
		int chunk_x = i / world_height_chunks;
		int chunk_y = i % world_height_chunks;

		auto& rect = chunk.rect;
		rect.x = chunk_x * chunk_width * tile_size;
		rect.y = chunk_y * chunk_height * tile_size;
		rect.w = chunk_width * tile_size;
		rect.h = chunk_height * tile_size;

		++i;
	}
}

void World::initTiles()
{
	tile_presets[BlockType::GRASS] = Tile{0, TileType::SURFACE, true, 100.f};
	tile_presets[BlockType::DIRT] = Tile{ 1, TileType::DIRT, true, 100.f };
	tile_presets[BlockType::STONE] = Tile{ 2, TileType::STONE, true, 125.f };
	tile_presets[BlockType::SAND] = Tile{ 3, TileType::SURFACE, true, 50.f };
	tile_presets[BlockType::SKY] = Tile{ 4, TileType::NONE, false, 0.f };
	tile_presets[BlockType::SNOW_GRASS] = Tile{ 5, TileType::SURFACE, true, 100.f };
	tile_presets[BlockType::SNOW_DIRT] = Tile{ 8, TileType::DIRT, true, 100.f };
	tile_presets[BlockType::ROCK] = Tile{ 6, TileType::STONE, true, 125.f };
	tile_presets[BlockType::WATER] = Tile{ 7, TileType::NONE, false, 0.f };
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
	forest.surface_tile = tile_presets.at(BlockType::GRASS);
	forest.dirt_tile = tile_presets.at(BlockType::DIRT);

	tundra.name = "Tundra";
	tundra.pv_min = 0.f;
	tundra.pv_max = 1.f;
	tundra.temperature_min = 0.0f;
	tundra.temperature_max = 0.4f;
	tundra.moisture_min = 0.f;
	tundra.moisture_max = 1.f;
	tundra.surface_tile = tile_presets.at(BlockType::SNOW_GRASS);
	tundra.dirt_tile = tile_presets.at(BlockType::SNOW_DIRT);

	desert.name = "Desert";
	desert.pv_min = 0.0f;
	desert.pv_max = 0.6f;
	desert.temperature_min = 0.8f;
	desert.temperature_max = 1.0f;
	desert.moisture_min = 0.0f;
	desert.moisture_max = 1.0f;
	desert.surface_tile = tile_presets.at(BlockType::SAND);
	desert.dirt_tile = tile_presets.at(BlockType::SAND);
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

void World::generateBase()
{
	for (int x = 0; x < width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;

		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(peaks_and_valleys_settings, position_x * scale);

		float map_height = peaks_and_valleys_map_range_height.getValue(peaks_and_valleys);

		float new_change = peaks_and_valleys_map_range_change.getValue(peaks_and_valleys);

		for (int y = 0; y < height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			float density_noise = Noise::fractal2D<ValueNoise>(density_settings, position_x * scale, position_y * scale);

			float new_height = position_y - map_height;

			//Apply y base layer and change over vertical axis
			float difference = new_height - y_base;
			float change = difference * new_change;
			density_noise += change;

			//Tile new_tile{};
				
			auto& tile = world_map(x, y);
			//tile.x = x % chunk_width_tiles;
			//tile.y = y % chunk_height_tiles;

			if (density_noise >= density_threshold)
			{
				//Solid tile
				//new_tile = tile_presets.at(BlockType::STONE);
				//tile.setTile(tile_presets.at(BlockType::STONE));
				tile.id = 0;
			}
			if (density_noise < density_threshold)
			{
				//Air tile
				//new_tile = tile_presets.at(BlockType::SKY);
				//tile.setTile(tile_presets.at(BlockType::SKY));
			}
		}
	}
}

void World::addGrass()
{
	for (int x = 0; x < width_tiles; x++)
	{
		bool was_air = false;
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = tiles[y + x * height_tiles];

			if (!tile.solid)
			{
				was_air = true;
				continue;
			}

			if (tile.solid && was_air)
			{
				tile.setTile(tile_presets[BlockType::GRASS]);
			}

			was_air = false;
		}
	}
}

void World::addDirt()
{
	for (int x = 0; x < width_tiles; x++)
	{
		bool found_surface = false;
		int surface_y = 0;

		float dirt_noise = ValueNoise::noise1D(static_cast<float>(x) * scale * 0.2f, seeds[2]) * 1.f;
		int dirt_level = static_cast<int>(dirt_noise * 5.f) + 5;

		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = tiles[y + x * height_tiles];

			if (tile.type == TileType::SURFACE)
			{
				found_surface = true;
				surface_y = y;
				continue;
			}

			if (found_surface && tile.solid)
			{
				int distance = y - surface_y;
				if (distance <= dirt_level)
				{
					tile.setTile(tile_presets[BlockType::DIRT]);
				}
			}

		}
	}
}

void World::addCaves()
{
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = tiles[y + x * height_tiles];

			float cave_noise = Noise::fractal2D<ValueNoise>(cave_settings, static_cast<float>(x) * scale, static_cast<float>(y) * scale);

			float c_difference = (cave_base_height - static_cast<float>(y)) * -1.f;
			float correlated_cave_threshold = std::clamp(caves_threshold_change.getValue(c_difference), 0.001f, cave_threshold_max);
	
			if (tile.solid && cave_noise < correlated_cave_threshold)
			{
				tile.setTile(tile_presets.at(BlockType::SKY));
				tile.sealed = true;
			}
		}
	}
}

void World::addWater()
{
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = tiles[y + x * height_tiles];

			if (!tile.solid && static_cast<float>(y) > sea_level && !tile.sealed)
			{
				tile.setTile(tile_presets.at(BlockType::WATER));
			}
		}
	}
}

void World::addBiomes()
{
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			auto& tile = tiles[y + x * height_tiles];

			if (tile.type != TileType::SURFACE && tile.type != TileType::DIRT) continue;

			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(peaks_and_valleys_settings, position_x * scale);
			float temperature = Noise::fractal2D<ValueNoise>(temperature_settings, position_x * scale, position_y * scale);
			float moisture = Noise::fractal2D<ValueNoise>(moisture_settings, position_x * scale, position_y * scale);

			//Desert,Forest,Snow
			if (temperature < 0.45f && moisture > 0.55f)
			{
				//Snow
				if (tile.type == TileType::SURFACE)
					tile.setTile(tundra.surface_tile);
				else
					tile.setTile(tundra.dirt_tile);
			}
			else if (temperature >= 0.6f && moisture < 0.5f)
			{
				//Desert
				tile.setTile(desert.surface_tile);
			}
			else
			{
				//Forest
				if (tile.type == TileType::SURFACE)
					tile.setTile(forest.surface_tile);
				else
					tile.setTile(forest.dirt_tile);
			}

#ifdef DEBUG_TILES
			tile.pv = std::min(peaks_and_valleys, 1.f);
			tile.temperature = std::min(temperature, 1.f);
			tile.moisture = std::min(moisture, 1.f);
#endif 
		}
	}
}

void World::addObjects()
{
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = tiles[y + x * height_tiles];

			//Tree
			if (tile == tile_presets[BlockType::GRASS])
			{
				//Check for space above
				bool is_space = true;
				for (int i = 1; i < 4; i++)
				{
					int new_y = y - i;
					if (new_y >= 0)
					{
						auto& tile_above = tiles[new_y + x * height_tiles];
						if (tile_above.solid)
						{
							is_space = false;
							break;
						}
					}
				}

				if (is_space)
				{
					Object tree{0,200.f};
					objects[glm::ivec2{ x,y-3 }] = tree;
				}
			}
		}
	}
}

void World::applyChanges()
{

}

void World::splitWorld()
{
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			int chunk_x = static_cast<int>(std::floor(x / chunk_width_tiles));
			int chunk_y = static_cast<int>(std::floor(y / chunk_height_tiles));

			int chunk_index = chunk_y + chunk_x * (height_tiles / chunk_height_tiles);

			//Tiles
			auto& tile = tiles[y + x * height_tiles];
			auto& chunk = chunks[chunk_index];
			chunk.addTile(tile);

			//Objects
			auto object_position = glm::ivec2{ x, y };
			if (objects.contains(object_position))
			{
				chunk.addObject(object_position, objects[object_position]);
			}
		}
	}
}

//void World::addSurface(std::vector<Tile>& tiles)
//{
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		float position_x = static_cast<float>(x);
//
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			if (!tile.solid) continue;
//
//			int top_local_y = tile_local_y - 1;
//
//			if (top_local_y >= 0)
//			{
//				auto& tile1 = chunk.tiles[top_local_y + tile_local_x * chunk_height_tiles];
//
//				if (!tile1.solid)
//				{
//					//Surface
//					tile.setTile(tile_presets.at(BlockType::GRASS));
//				}
//			}
//			else
//			{
//				if (!isTileSolid(x, y - 1))
//				{
//					//Surface
//					tile.setTile(tile_presets.at(BlockType::GRASS));
//				}
//			}
//		}
//	}
//}
//
//void World::addDirt(Chunk& chunk)
//{
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		float position_x = static_cast<float>(x);
//
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			if (!tile.solid) continue;
//
//			float dirt_noise = ValueNoise::noise1D(position_x * scale * 0.2f, seeds[2]) * 1.f;
//
//			int dirt_level = static_cast<int>(dirt_noise * 5.f) + 5;
//
//			for (int i = 0; i < dirt_level; ++i)
//			{
//				int new_local_y = tile_local_y - i;
//				int new_y = y - i;
//
//				if (new_local_y >= 1)
//				{
//					auto& tile1 = chunk.tiles[new_local_y + tile_local_x * chunk_height_tiles];
//					auto& tile2 = chunk.tiles[new_local_y - 1 + tile_local_x * chunk_height_tiles];
//
//					if (tile1.solid && !tile2.solid)
//					{
//						tile.setTile(tile_presets.at(BlockType::DIRT));
//						break;
//					}
//				}
//				else
//				{
//					if (isTileSolid(x, new_y) && !isTileSolid(x, new_y - 1))
//					{
//						tile.setTile(tile_presets.at(BlockType::DIRT));
//						break;
//					}
//				}
//			}
//		}
//	}
//}
//
//void World::addWater(Chunk& chunk)
//{
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_x = static_cast<float>(x);
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			if (!tile.solid && position_y > sea_level && !tile.sealed)
//			{
//				tile.setTile(tile_presets.at(BlockType::WATER));
//			}
//		}
//	}
//}
//
//void World::addCaves(Chunk& chunk)
//{
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_x = static_cast<float>(x);
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			//Cave noise
//			float cave_noise = Noise::fractal2D<ValueNoise>(cave_settings, position_x * scale, position_y * scale);
//
//			float c_difference = (cave_base_height - position_y) * -1.f;
//			float correlated_cave_threshold = std::clamp(caves_threshold_change.getValue(c_difference), 0.001f, cave_threshold_max);
//
//			if (tile.solid && cave_noise < correlated_cave_threshold)
//			{
//				tile.setTile(tile_presets.at(BlockType::SKY));
//				tile.sealed = true;
//			}
//		}
//	}
//}
//
//void World::addTunnels(Chunk& chunk)
//{
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_x = static_cast<float>(x);
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			float tunnel_noise = Noise::fractal2D<ValueNoise>(tunnel_settings, position_x * scale, position_y * scale);
//
//			if (tile.solid)
//			{
//				if (tunnel_noise > tunnel_threshold_min && tunnel_noise < tunnel_threshold_max)
//				{
//					tile.setTile(tile_presets.at(BlockType::SKY));
//				}
//			}
//		}
//	}
//}
//
//void World::addBiomes(Chunk& chunk)
//{ 
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_x = static_cast<float>(x);
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			if (tile.type != TileType::SURFACE && tile.type != TileType::DIRT) continue;
//
//			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(peaks_and_valleys_settings, position_x * scale);
//			float temperature = Noise::fractal2D<ValueNoise>(temperature_settings, position_x * scale, position_y * scale);
//			float moisture = Noise::fractal2D<ValueNoise>(moisture_settings, position_x * scale, position_y * scale);
//
//			//Desert,Forest,Snow
//			if (temperature < 0.45f && moisture > 0.55f)
//			{
//				//Snow
//				if (tile.type == TileType::SURFACE)
//					tile.setTile(tundra.surface_tile);
//				else
//					tile.setTile(tundra.dirt_tile);
//			}
//			else if (temperature >= 0.6f && moisture < 0.5f)
//			{
//				//Desert
//				tile.setTile(desert.surface_tile);
//			}
//			else
//			{
//				//Forest
//				if (tile.type == TileType::SURFACE)
//					tile.setTile(forest.surface_tile);
//				else
//					tile.setTile(forest.dirt_tile);
//			}
//
//#ifdef DEBUG_TILES
//			tile.pv = std::min(peaks_and_valleys, 1.f);
//			tile.temperature = std::min(temperature, 1.f);
//			tile.moisture = std::min(moisture, 1.f);
//#endif 
//		}
//	}
//}
//
//void World::addObjects(Chunk& chunk)
//{
//	auto s = object_manager.lock();
//	if (!s) return;
//	
//	for (int x = chunk.x; x < chunk.x + chunk_width_tiles; x++)
//	{
//		for (int y = chunk.y; y < chunk.y + chunk_height_tiles; y++)
//		{
//			float position_x = static_cast<float>(x);
//			float position_y = static_cast<float>(y);
//
//			int tile_local_x = x - chunk.x;
//			int tile_local_y = y - chunk.y;
//
//			auto& tile = chunk.tiles[tile_local_y + tile_local_x * chunk_height_tiles];
//
//			if (tile == tile_presets[BlockType::GRASS])
//			{
//				chunk.addObject(Object{0, 200.f});
//			}
//		}
//	}
//}
//
//void World::applyChanges(Chunk& chunk)
//{
//	auto it = changes.find({chunk.index_x, chunk.index_y});
//
//	if (it != changes.end())
//	{
//		const auto& vec_changes = it->second;
//
//		for (const auto& change : vec_changes)
//		{
//			chunk.tiles[change.local_y + change.local_x * chunk_height_tiles] = change;
//		}
//	}
//}
