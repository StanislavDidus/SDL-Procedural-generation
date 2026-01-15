#include "World.hpp"
#include "Debug.hpp"
#include "ECS/Systems.hpp"

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

//Returns true if a rect is completely inside b
static bool isRectInsideFloat(const SDL_FRect& a, const SDL_FRect& b)
{
	return a.x >= b.x &&
		   a.x + a.w <= b.x + b.w &&
		   a.y >= b.y &&
	   	   a.y + a.h <= b.y + b.h;
}

World::World
(
	const GenerationData& generation_data,
	const SpriteSheet& tileset,
	const SpriteSheet& object_spritesheet,
	std::shared_ptr<CollisionSystem> collision_system,
	std::shared_ptr<ObjectManager> object_manager,
	int width_tiles, int height_tiles,
	float tile_width_world, float tile_height_world
)
	: generation_data(generation_data)
	, tileset(tileset)
	, object_spritesheet(object_spritesheet)
	, world_map(height_tiles, width_tiles)
	, width_tiles(width_tiles)
	, height_tiles(height_tiles)
	, collision_system(collision_system)
	, object_manager(object_manager)
	, tile_width_world(tile_width_world)
	, tile_height_world(tile_height_world)
	, world_width_chunks(static_cast<float>(width_tiles) / chunk_width_tiles)
	, world_height_chunks(static_cast<float>(height_tiles) / chunk_height_tiles)
{
	
}

void World::setCollisionSystem(std::shared_ptr<CollisionSystem> collision_system)
{
	this->collision_system = collision_system;
}

void World::update(const Renderer& screen, float dt, const glm::vec2& target)
{
	const auto& window_size = screen.getWindowSize();
	const auto& view_position = screen.getView();
	auto zoom = screen.getZoom();

	float halfW = (window_size.x * 0.5f) / zoom;
	float halfH = (window_size.y * 0.5f) / zoom;

	float left_world = target.x - halfW;
	float right_world = target.x + halfW;
	int begin_x = static_cast<int>(std::floor(left_world / 20.f));
	int end_x = static_cast<int>(std::ceil(right_world / 20.f));

	float up_world = target.y - halfH;
	float down_world = target.y + halfH;
	int begin_y = static_cast<int>(std::floor(up_world / 20.f));
	int end_y = static_cast<int>(std::ceil(down_world / 20.f));

	camera_rect.x = left_world;
	camera_rect.y = up_world;
	camera_rect.w = right_world - left_world;
	camera_rect.h = down_world - up_world;
}

void World::render(Renderer& screen) const
{
	float chunk_width_tiles = 25.f;
	float chunk_height_tiles = 25.f;

	//Render tiles
	for (const auto& chunk : chunks)
	{
		if (SDL_HasRectIntersectionFloat(&camera_rect, &chunk.rect))
		{
			for (int x = 0; x < chunk.tilemap.getColumns(); ++x)
			{
				for (int y = 0; y < chunk.tilemap.getRows(); ++y)
				{
					auto& tile = chunk.tilemap(x, y);

					SDL_FRect tile_rect;
					tile_rect.x = chunk.rect.x + x * 20.f;
					tile_rect.y = chunk.rect.y + y * 20.f;
					tile_rect.w = 20.f;
					tile_rect.h = 20.f;

					if (SDL_HasRectIntersectionFloat(&camera_rect, &tile_rect))
					{

						int sprite_index = getTileProperties(tile.id).sprites_index;
						int is_solid = getTileProperties(tile.id).is_solid;

						screen.drawScaledSprite(tileset[sprite_index], tile_rect.x, tile_rect.y, 20.f, 20.f);

						//Add collisions
						if (auto s = collision_system.lock())
						{
							if (is_solid) s->collisions.emplace_back(glm::ivec2{ static_cast<int>(tile_rect.x), static_cast<int>(tile_rect.y) }, glm::ivec2{ 20.f, 20.f });
						}
					}
				}
			}
		}
	}

	//Render objects
	for (const auto& chunk : chunks)
	{
		if (SDL_HasRectIntersectionFloat(&camera_rect, &chunk.rect))
		{
			for (const auto& object : chunk.objects)
			{
				const auto& object_size = generation_data.object_manager->getProperties(object.id).size;
				int sprite_index = generation_data.object_manager->getProperties(object.id).sprite_index;
				screen.drawScaledSprite(object_spritesheet[sprite_index], object.position.x, object.position.y, object_size.x, object_size.y);
			}
		}
	}
}

void World::placeTile(int tile_x, int tile_y, BlockType block)
{
	if (tile_x < 0 || tile_x > width_tiles - 1 | tile_y < 0 || tile_y > height_tiles - 1) return;

	auto& tile = world_map(tile_x, tile_y);
	auto& tile_manager = generation_data.tile_manager;

	if (auto& tile_properties = tile_manager->getProperties(tile.id); !tile_properties.is_solid)
	{
		//Check adjacent tiles
		bool is_placement_allowed = false;
		for (int i = -1; i < 2; ++i) for (int j = -1; j < 2; ++j)
		{
			//Don't check the middle tile (it is where we want to place a new tile)
			if (i == 0 && j == 0) continue;

			auto& tile_temp = world_map(tile_x + i, tile_y + j);
			auto& properties_temp = tile_manager->getProperties(tile_temp.id);
			if (properties_temp.is_solid)
			{
				is_placement_allowed = true;
				break;
			}
		}

		if (is_placement_allowed)
		{
			tile.id = generation_data.tiles[block];

			//Mark tile dirty
			dirt_tiles.emplace_back(tile.id, glm::ivec2{ tile_x, tile_y });
		}
	}
}

void World::damageTile(int tile_x, int tile_y, float damage)
{	
	if (tile_x < 0 || tile_x > width_tiles - 1 | tile_y < 0 || tile_y > height_tiles - 1) return;

	auto& tile = world_map(tile_x, tile_y);

	if (auto& tile_properties = generation_data.tile_manager->getProperties(tile.id); tile_properties.is_solid && !tile.attached)
	{
		tile.dealDamage(damage);

		if (tile.is_destroyed)
		{
			tile.id = generation_data.tiles[BlockType::SKY];
			tile.is_destroyed = false;
		}

		//Mark tile dirty
		dirt_tiles.emplace_back(tile.id, glm::ivec2{ tile_x, tile_y });
	}
}

Object* World::getObjectOnPosition(const glm::vec2& mouse_global_position)
{
	int chunk_x_index = static_cast<int>(std::floor(mouse_global_position.x / (static_cast<float>(chunk_width_tiles) * 20.f)));
	int chunk_y_index = static_cast<int>(std::floor(mouse_global_position.y / (static_cast<float>(chunk_height_tiles) * 20.f)));

	if (chunk_x_index < 0 || chunk_x_index >= world_width_chunks ||
		chunk_y_index < 0 || chunk_y_index >= world_height_chunks) return nullptr;

	auto& chunk = chunks.at(static_cast<size_t>(static_cast<float>(chunk_y_index) + static_cast<float>(chunk_x_index) * world_height_chunks));

	for (auto& object : chunk.objects)
	{
		const auto& object_position = object.position;
		const auto& object_size = generation_data.object_manager->getProperties(object.id).size;

		if (mouse_global_position.x > object_position.x &&
			mouse_global_position.x < object_position.x + object_size.x &&
			mouse_global_position.y > object_position.y &&
			mouse_global_position.y < object_position.y + object_size.y)
		{
			return &object;
		}
	}

	return nullptr;
}

std::optional<int> World::damageObject(const glm::vec2& mouse_global_position, float damage)
{
	/*int tile_x_index = static_cast<int>(std::floor(mouse_global_position.x / tile_width_world));
	int tile_y_index = static_cast<int>(std::floor(mouse_global_position.y / tile_height_world));

	auto& chunk = tilePositionToChunk({ tile_x_index, tile_y_index });
	auto* object = getObjectOnPosition(mouse_global_position);

	if (object)
	{
		object->dealDamage(damage);

		if (object->is_destroyed)
		{
			std::erase_if(chunk.objects, [object](const Object& other) { return object->id == other.id; });
		}
	}*/

	int chunk_x_index = static_cast<int>(std::floor(mouse_global_position.x / (chunk_width_tiles * 20.f)));
	int chunk_y_index = static_cast<int>(std::floor(mouse_global_position.y / (chunk_height_tiles * 20.f)));

	int height_chunks = height_tiles / chunk_height_tiles;
	int width_chunks = width_tiles / chunk_width_tiles;

	if (chunk_x_index < 0 || chunk_x_index >= width_chunks ||
		chunk_y_index < 0 || chunk_y_index >= height_chunks) return std::nullopt;

	auto& chunk = chunks[chunk_y_index + chunk_x_index * height_chunks];

	for (int i = 0; auto& object : chunk.objects)
	{
		const auto& object_position = object.position;
		const auto& object_size = generation_data.object_manager->getProperties(object.id).size;

		if (mouse_global_position.x > object_position.x &&
			mouse_global_position.x < object_position.x + object_size.x &&
			mouse_global_position.y > object_position.y &&
			mouse_global_position.y < object_position.y + object_size.y)
		{
			object.dealDamage(damage);

			if (object.is_destroyed)
			{
				chunk.objects.erase(chunk.objects.begin() + i);
				return object.id;
				//inventory.addItem(generation_data.object_manager->getProperties(object.id).item);
			}
			//	//Remove attached mark from the tile beneath the object
			//	float new_x = static_cast<float>(object_position.x / 20.f);
			//	float new_y = static_cast<float>(object_position.y / 20.f + object_size.y / 20.f + 1.f);
			//	int chunk_x_index = static_cast<int>(std::floor(new_x / static_cast<float>(chunk_width_tiles)));
			//	int chunk_y_index = static_cast<int>(std::floor(new_y / static_cast<float>(chunk_height_tiles)));

			//	if (chunk_x_index < 0 || chunk_x_index >= width_chunks ||
			//		chunk_y_index < 0 || chunk_y_index >= height_chunks) continue;

			//	int tile_local_x = x % chunk_width_tiles;
			//	int tile_local_y = y % chunk_height_tiles;

			//	int chunk_index_ = chunk_x_index + chunk_y_index * width_chunks;
			//	auto& chunk_ = chunks[chunk_index_];
			//	auto& tile = chunk_.tilemap();
			//}
		}

		++i;
	}

	return std::nullopt;
}

std::optional<ObjectProperties> World::getProperties(int id) const
{
	if (auto s = object_manager.lock())
	{
		return s->getProperties(id);
	}
	return std::nullopt;
}

const TileProperties& World::getTileProperties(int id) const
{
	return generation_data.tile_manager->getProperties(id);
}

void World::rebuildChunks()
{
	for (const auto& dirt_tile : dirt_tiles)
	{
		auto& chunk = tilePositionToChunk(dirt_tile.position);
		auto tile_local_position = getTileLocalPosition(dirt_tile.position);
		chunk.tilemap(tile_local_position.x, tile_local_position.y).id = dirt_tile.id;
	}

	dirt_tiles.clear();
}

void World::updateTiles()
{
	for (auto& chunk : chunks)
	{
		for (auto& object : chunk.objects)
		{
			float max_durability = generation_data.object_manager->getProperties(object.id).durability;
			if (object.current_durability < max_durability && !object.received_damage_last_frame)
			{
				object.current_durability = max_durability;
			}

			object.received_damage_last_frame = false;
		}
	}

	for (int x = 0; x < world_map.getColumns(); ++x)
	{
		for (int y = 0; y < world_map.getRows(); ++y)
		{
			auto& tile = world_map(x, y);

			float max_durability = generation_data.tile_manager->getProperties(tile.id).max_durability;
			if (tile.current_durability < max_durability && !tile.received_damage_last_frame)
			{
				tile.current_durability = max_durability;
			}

			tile.received_damage_last_frame = false;
		}
	}
}

void World::generateWorld(std::optional<int> seed)
{
	world_map.reset();
	objects.clear();

	initSeeds(seed);

	//tiles.resize(width_tiles * height_tiles);
	//chunks.resize(width_tiles / chunk_width_tiles * height_tiles / chunk_height_tiles);

	generateBase();
	addGrass();
	addDirt();
	addCaves();
	addWater();
	addBiomes();
	addObjects();
	splitGrid(world_map, chunk_width_tiles, chunk_height_tiles);
}

void World::splitGrid(const Grid<Tile>& grid, int chunk_width, int chunk_height)
{
	chunks.clear();

	int width = grid.getColumns();
	int height = grid.getRows();

	int world_width_chunks = width / chunk_width;
	int world_height_chunks = height / chunk_height;

	chunks.reserve(world_width_chunks * world_height_chunks);

	//Fill chunks with empty tiles
	for (int i = 0; i < world_width_chunks * world_height_chunks; ++i)
	{
		chunks.emplace_back(tileset, SDL_FRect{}, chunk_height, chunk_width);
	}

	//Divide tiles between all chunks
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int tile_local_x = x % chunk_width;
			int tile_local_y = y % chunk_height;

			int chunk_x = x / chunk_width;
			int chunk_y = y / chunk_height;

			auto& chunk = chunks[chunk_y + chunk_x * world_height_chunks];
			chunk.tilemap(tile_local_x, tile_local_y) = grid(x, y);
		}
	}

	float tile_size = 20.f;

	//Precalculate the rect of each chunk
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

	//Divide objects between chunks
	for (auto& chunk : chunks)
	{
		for (const auto& object : objects)
		{
			const auto& object_size = generation_data.object_manager->getProperties(object.id).size;
			SDL_FRect object_rect{ object.position.x, object.position.y, object_size.x, object_size.y };
			if (isRectInsideFloat(object_rect, chunk.rect))
			{
				chunk.objects.push_back(object);
			}
		}
	}
}

Chunk& World::tilePositionToChunk(const glm::ivec2 tile_position)
{
	glm::ivec2 chunk_position =
	{
		static_cast<int>(static_cast<float>(tile_position.x) / static_cast<float>(chunk_width_tiles)),
		static_cast<int>(static_cast<float>(tile_position.y) / static_cast<float>(chunk_height_tiles))
	};

	auto& chunk = chunks.at(static_cast<int>(std::floor(chunk_position.y + chunk_position.x * world_height_chunks)));

	return chunk;
}

glm::ivec2 World::getTileLocalPosition(const glm::ivec2 tile_position) const
{
	glm::ivec2 tile_local_position
	{
		tile_position.x % chunk_width_tiles,
		tile_position.y % chunk_height_tiles
	};

	return tile_local_position;
}

void World::initSeeds(std::optional<int> seed_opt)
{
	int seed = 0;
	if (!seed_opt.has_value())
	{
		std::uniform_int_distribution dist(0, 10000000);
		seed = dist(rng);
	}
	else
	{
		seed = seed_opt.value();
	}

	/*for (int i = 1; i < seeds.size(); ++i)
	{
		//seeds[i] = seeds[0] + i;
	}*/

	for (int i = 0; auto& [type, noise] : generation_data.noise_settings)
	{
		noise.seed = seed + i;
		++i;
	}

}

void World::generateBase()
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;

		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(generation_data.noise_settings[NoiseType::PV], position_x * scale);

		float map_height = generation_data.maps[MapRangeType::PV_HEIGHT].getValue(peaks_and_valleys);

		float new_change = generation_data.maps[MapRangeType::PV_CHANGE].getValue(peaks_and_valleys);

		for (int y = 0; y < height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			float density_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::DENSITY], position_x * scale, position_y * scale);

			float new_height = position_y - map_height;

			//Apply y base layer and change over vertical axis
			float difference = new_height - generation_data.y_base;
			float change = difference * new_change;
			density_noise += change;

			//Tile new_tile{};
				
			auto& tile = world_map(x, y);
			//tile.x = x % chunk_width_tiles;
			//tile.y = y % chunk_height_tiles;

			if (density_noise >= generation_data.density_threshold)
			{
				//Solid tile
				tile.id = generation_data.tiles.at(BlockType::STONE);
			}
			if (density_noise < generation_data.density_threshold)
			{
				//Air tile
				tile.id = generation_data.tiles.at(BlockType::SKY);
			}
		}
	}
}

void World::addGrass()
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		bool was_air = false;
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = world_map(x, y);
			bool is_solid = generation_data.tile_manager->getProperties(tile.id).is_solid;

			if (!is_solid)
			{
				was_air = true;
				continue;
			}

			if (is_solid && was_air)
			{
				tile.id = generation_data.tiles.at(BlockType::GRASS);
			}

			was_air = false;
		}
	}
}

void World::addDirt()
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		bool found_surface = false;
		int surface_y = 0;

		float dirt_noise = Noise::fractal1D<ValueNoise>(generation_data.noise_settings[NoiseType::DIRT], static_cast<float>(x) * scale);
		//float dirt_noise = ValueNoise::noise1D(static_cast<float>(x) * scale * 0.2f, seeds[2]) * 1.f;
		int dirt_level = static_cast<int>(dirt_noise * 5.f) + 5;

		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = world_map(x, y);
			TileType type = generation_data.tile_manager->getProperties(tile.id).type;
			bool is_solid = generation_data.tile_manager->getProperties(tile.id).is_solid;

			if (type == TileType::SURFACE)
			{
				found_surface = true;
				surface_y = y;
				continue;
			}

			if (found_surface && is_solid)
			{
				int distance = y - surface_y;
				if (distance <= dirt_level)
				{
					tile.id = generation_data.tiles.at(BlockType::DIRT);
				}
			}

		}
	}
}

void World::addCaves()
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = world_map(x, y);
			bool is_solid = generation_data.tile_manager->getProperties(tile.id).is_solid;

			float cave_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::CAVE], static_cast<float>(x) * scale, static_cast<float>(y) * scale);

			float c_difference = (generation_data.cave_y_base - static_cast<float>(y)) * -1.f;
			auto& change_map = generation_data.maps[MapRangeType::CAVE_CHANGE];
			float correlated_cave_threshold = std::clamp(change_map.getValue(c_difference), 0.001f, change_map.getOutMax());
	
			if (is_solid&& cave_noise < correlated_cave_threshold)
			{
				tile.id = tile.id = generation_data.tiles.at(BlockType::SKY);
				tile.sealed = true;
			}
		}
	}
}

void World::addWater()
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			auto& tile = world_map(x, y);
			bool is_solid = generation_data.tile_manager->getProperties(tile.id).is_solid;

			if (!is_solid && static_cast<float>(y) > generation_data.sea_y_base && !tile.sealed)
			{
				tile.id = tile.id = generation_data.tiles.at(BlockType::WATER);
			}
		}
	}
}

void World::addBiomes()
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		for (int y = 0; y < height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			auto& tile = world_map(x, y);
			TileType type = generation_data.tile_manager->getProperties(tile.id).type;

			if (type != TileType::SURFACE && type != TileType::DIRT) continue;

			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(generation_data.noise_settings[NoiseType::PV], position_x * scale);
			float temperature = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::TEMPERATURE], position_x * scale, position_y * scale);
			float moisture = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::MOISTURE], position_x * scale, position_y * scale);

			//Desert,Forest,Snow
			if (temperature < 0.45f && moisture > 0.55f)
			{
				//Snow
				if (type == TileType::SURFACE)
					tile.id = generation_data.biomes[BiomeType::TUNDRA].surface_tile;
				else
					tile.id = generation_data.biomes[BiomeType::TUNDRA].dirt_tile;
			}
			else if (temperature >= 0.6f && moisture < 0.5f)
			{
				//Desert
				tile.id = generation_data.biomes[BiomeType::DESERT].surface_tile;
			}
			else
			{
				//Forest
				if (type == TileType::SURFACE)
					tile.id = generation_data.biomes[BiomeType::FOREST].surface_tile;
				else		  
					tile.id = generation_data.biomes[BiomeType::FOREST].dirt_tile;
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
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		for (int y = 0; y < height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			auto& tile = world_map(x, y);

			float tree_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::TREES], position_x * scale, position_y * scale);
			if (tree_noise > generation_data.tree_threshold)
			{
				//Tree
				if (tile.id == generation_data.tiles.at(BlockType::GRASS))
				{
					//Check for space above
					bool is_space = true;
					for (int i = 1; i < 4; i++)
					{
						int new_y = y - i;
						if (new_y >= 0)
						{
							auto& tile_above = world_map(x, new_y);
							bool is_solid = generation_data.tile_manager->getProperties(tile_above.id).is_solid;
							if (is_solid)
							{
								is_space = false;
								break;
							}
						}
					}

					if (is_space)
					{
						glm::vec2 position = { position_x * 20.f, (position_y - 3.f) * 20.f };
						objects.emplace_back(0, 200.f, position);

						//Make the tile attached
						//tile.attached = true;
					}
				}

				//Snow Tree
				if (tree_noise > generation_data.tree_threshold)
				{
					if (tile.id == generation_data.tiles.at(BlockType::SNOW_GRASS))
					{
						//Check for space above
						bool is_space = true;
						for (int i = 1; i < 4; i++)
						{
							int new_y = y - i;
							if (new_y >= 0)
							{
								auto& tile_above = world_map(x, new_y);
								bool is_solid = generation_data.tile_manager->getProperties(tile_above.id).is_solid;
								if (is_solid)
								{
									is_space = false;
									break;
								}
							}
						}

						if (is_space)
						{
							glm::vec2 position = { position_x * 20.f, (position_y - 3.f) * 20.f };
							objects.emplace_back(1, 200.f, position);

							//Make the tile attached
							//tile.attached = true;
						}
					}
				}

				//Kaktus
				if (tree_noise > generation_data.tree_threshold)
				{
					if (tile.id == generation_data.tiles.at(BlockType::SAND))
					{
						//Check for space above
						bool is_space = true;
						for (int i = 1; i < 4; i++)
						{
							int new_y = y - i;
							if (new_y >= 0)
							{
								auto& tile_above = world_map(x, new_y);
								bool is_solid = generation_data.tile_manager->getProperties(tile_above.id).is_solid;
								if (is_solid)
								{
									is_space = false;
									break;
								}
							}
						}

						if (is_space)
						{
							glm::vec2 position = { position_x * 20.f, (position_y - 3.f) * 20.f };
							objects.emplace_back(2, 200.f, position);

							//Make the tile attached
							//tile.attached = true;
						}
					}
				}
			}
		}
	}
}

void World::applyChanges()
{

}
