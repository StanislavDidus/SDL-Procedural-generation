#include "World.hpp"
#include "Debug.hpp"
#include "ECS/Systems.hpp"

#include <random>
#include <ranges>

#include "ResourceManager.hpp"

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
	std::shared_ptr<CollisionSystem> collision_system,
	int width_tiles, int height_tiles,
	float tile_width_world, float tile_height_world
)
	: generation_data(generation_data)
	, world_map(height_tiles, width_tiles)
	, width_tiles(width_tiles)
	, height_tiles(height_tiles)
	, collision_system(collision_system)
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

void World::update(const graphics::Renderer& screen, float dt, const glm::vec2& target)
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

void World::render(graphics::Renderer& screen) const
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

						graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("tiles")[sprite_index], tile_rect.x, tile_rect.y, 20.f, 20.f);

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
				if (SDL_HasRectIntersectionFloat(&camera_rect, &object.rect))
				{
					int sprite_index = ObjectManager::get().getProperties(object.properties_id).sprite_index;
					graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("objects")[sprite_index], object.rect.x, object.rect.y, object.rect.w, object.rect.h);
				}
			}
		}
	}
}

void World::placeTile(int tile_x, int tile_y, size_t tile_id)
{
	if (tile_x < 0 || tile_x > width_tiles - 1 | tile_y < 0 || tile_y > height_tiles - 1) return;

	auto& tile = world_map(tile_x, tile_y);
	auto& tile_manager = TileManager::get();

	if (auto& tile_properties = tile_manager.getProperties(tile.id); !tile_properties.is_solid)
	{
		//Check adjacent tiles
		bool is_placement_allowed = false;
		for (int i = -1; i < 2; ++i) for (int j = -1; j < 2; ++j)
		{
			//Don't check the middle tile (it is where we want to place a new tile)
			if (i == 0 && j == 0) continue;

			int new_x = tile_x + i;
			int new_y = tile_y + j;

			if (new_x < 0 || new_x >= width_tiles || new_y < 0 || new_y >= height_tiles) continue;

			auto& tile_temp = world_map(new_x, new_y);
			auto& properties_temp = tile_manager.getProperties(tile_temp.id);
			if (properties_temp.is_solid)
			{
				is_placement_allowed = true;
				break;
			}
		}

		if (is_placement_allowed)
		{
			tile.id = tile_id;

			//Mark tile dirty
			dirt_tiles.emplace_back(tile.id, glm::ivec2{ tile_x, tile_y });
		}
	}
}

void World::damageTile(int tile_x, int tile_y, float damage)
{
	if (tile_x < 0 || tile_x > width_tiles - 1 | tile_y < 0 || tile_y > height_tiles - 1) return;

	auto& tile = world_map(tile_x, tile_y);

	if (auto& tile_properties = TileManager::get().getProperties(tile.id); tile_properties.is_solid && !tile.attached)
	{
		tile.dealDamage(damage);

		if (tile.is_destroyed)
		{
			tile.id = TileManager::get().getTileID("Sky");
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

	SDL_FRect mouse_rect = { mouse_global_position.x, mouse_global_position.y, 1.f, 1.f };

	for (auto& object : chunk.objects)
	{
		if (SDL_HasRectIntersectionFloat(&mouse_rect, &object.rect))
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

	SDL_FRect mouse_rect = { mouse_global_position.x, mouse_global_position.y, 1.f, 1.f };

	for (auto& object : chunk.objects)
	{
		int object_properties_id = object.properties_id;
		const auto& object_rect = object.rect;

		if (SDL_HasRectIntersectionFloat(&mouse_rect, &object_rect))
		{
			object.dealDamage(damage);

			if (object.is_destroyed)
			{
				std::erase_if(chunk.objects, [&object](const Object& obj) {return object == obj; });
				return object_properties_id;

			}
		}
	}

	return std::nullopt;
}

std::optional<ObjectProperties> World::getProperties(int id) const
{

	return ObjectManager::get().getProperties(id);
	
	return std::nullopt;
}

const TileProperties& World::getTileProperties(int id) const
{
	return TileManager::get().getProperties(id);
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
			float max_durability = ObjectManager::get().getProperties(object.properties_id).durability;
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

			float max_durability = TileManager::get().getProperties(tile.id).max_durability;
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

	initSeeds(seed);

	//tiles.resize(width_tiles * height_tiles);
	//chunks.resize(width_tiles / chunk_width_tiles * height_tiles / chunk_height_tiles);

	generateBase();
	addGrass();
	addDirt();
	addCaves();
	//addWater();
	addBiomes();

	std::vector<Object> objects;
	addObjects(objects);
	splitGrid(world_map, objects, chunk_width_tiles, chunk_height_tiles);
}

void World::splitGrid(const Grid<Tile>& grid, const std::vector<Object>& objects, int chunk_width, int chunk_height)
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
		chunks.emplace_back(ResourceManager::get().getSpriteSheet("tiles"), SDL_FRect{}, chunk_height, chunk_width);
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

	//Precalculate the rect of each chunk
	for (int i = 0; auto& chunk : chunks)
	{
		int chunk_x = i / world_height_chunks;
		int chunk_y = i % world_height_chunks;

		auto& rect = chunk.rect;
		rect.x = chunk_x * chunk_width * tile_width_world;
		rect.y = chunk_y * chunk_height * tile_height_world;
		rect.w = chunk_width * tile_width_world;
		rect.h = chunk_height * tile_height_world;

		++i;
	}

	//Divide objects between chunks
	for (auto& chunk : chunks)
	{
		for (const auto& object : objects)
		{
			const auto& object_rect = object.rect;
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
	seeds_count = 0;
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


	for (auto& noise : generation_data.noise_settings | std::views::values)
	{
		noise.seed = getNewSeed(seed);
	}

	for (auto& object : ObjectManager::get().getAllObjectSpawnInfos())
	{
		// TODO: Set the seeds for ObjectSpawnInfo before generation them!

		object.noise_settings.seed = getNewSeed(seed);
	}

}

uint32_t World::getNewSeed(uint32_t master_seed)
{
	return master_seed + static_cast<uint32_t>(seeds_count++);	
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

			auto& tile = world_map(x, y);

			if (density_noise >= generation_data.density_threshold)
			{
				//Solid tile
				tile.id = TileManager::get().getTileID("Stone");
			}
			if (density_noise < generation_data.density_threshold)
			{
				//Air tile
				tile.id = TileManager::get().getTileID("Sky");
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
			bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

			if (!is_solid)
			{
				was_air = true;
				continue;
			}

			if (is_solid && was_air)
			{
				tile.id = TileManager::get().getTileID("Grass");
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
			TileType type = TileManager::get().getProperties(tile.id).type;
			bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

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
					tile.id = TileManager::get().getTileID("Dirt");
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
			bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

			float cave_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::CAVE], static_cast<float>(x) * scale, static_cast<float>(y) * scale);

			float c_difference = (generation_data.cave_y_base - static_cast<float>(y)) * -1.f;
			auto& change_map = generation_data.maps[MapRangeType::CAVE_CHANGE];
			float correlated_cave_threshold = std::clamp(change_map.getValue(c_difference), 0.001f, change_map.getOutMax());

			if (is_solid && cave_noise < correlated_cave_threshold)
			{
				tile.id = TileManager::get().getTileID("Sky");
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
			bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

			if (!is_solid && static_cast<float>(y) > generation_data.sea_y_base && !tile.sealed)
			{
				tile.id = TileManager::get().getTileID("Water");
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
			TileType type = TileManager::get().getProperties(tile.id).type;

			if (type != TileType::SURFACE && type != TileType::DIRT) continue;

			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(generation_data.noise_settings[NoiseType::PV], position_x * scale);
			float temperature = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::TEMPERATURE], position_x * scale, position_y * scale);
			float moisture = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::MOISTURE], position_x * scale, position_y * scale);

			for (const auto& biome : generation_data.biomes)
			{
				if (peaks_and_valleys >= biome.pv_min && peaks_and_valleys < biome.pv_max)
				{
					if (temperature >= biome.temperature_min && temperature < biome.temperature_max)
					{
						if (moisture >= biome.moisture_min && moisture < biome.moisture_max)
						{
							if (type == TileType::SURFACE)
							{
								tile.id = biome.surface_tile_id;
								break;
							}
							else if (type == TileType::DIRT)
							{
								tile.id = biome.dirt_tile_id;
								break;
							}
						}
					}
				}
			}

			/*//Desert,Forest,Snow
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
			}*/

#ifdef DEBUG_TILES
			tile.pv = std::min(peaks_and_valleys, 1.f);
			tile.temperature = std::min(temperature, 1.f);
			tile.moisture = std::min(moisture, 1.f);
#endif 
		}
	}
}

void World::addObjects(std::vector<Object>& objects)
{
	float scale = generation_data.scale;
	for (int x = 0; x < width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		for (int y = 0; y < height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			const auto& object_spawn_infos = ObjectManager::get().getAllObjectSpawnInfos();

			for (const auto& spawn_info : object_spawn_infos)
			{
				//Check all 4 conditions

				//First - Noise
				float noise_value = Noise::fractal2D<ValueNoise>(spawn_info.noise_settings, position_x * scale, position_y * scale);
				if (noise_value < spawn_info.noise_threshold) continue;

				//Second - Tile type
				bool is_area_good = true;
				for (int i = 0; i < spawn_info.size_tiles.x; ++i)
				{
					bool is_tile_good = false;
					auto& new_tile = world_map(x + i, y);
					for (const auto& required_tile_id : spawn_info.spawn_tile_ids)
					{
						if (new_tile.id == required_tile_id)
						{
							is_tile_good = true;
							break;
						}
					}
					if (!is_tile_good)
					{
						is_area_good = false;
						break;
					}
				}
				if (!is_area_good) continue;

				//Third - Object Size
				auto& object_size = spawn_info.size_tiles;

				bool is_space_free = true;
				for (int i = 0; i < object_size.x; ++i)
				{
					//Set a small 1 tile offset for y loop because we want to check tiles above the ground
					for (int j = 1; j <= object_size.y; ++j)
					{

						//Check the boundaries of thw world before getting a tile from the world_map
						int new_x = x + i;
						int new_y = y - j;

						if (new_x < 0 || new_x >= width_tiles || new_y < 0 || new_y >= height_tiles)
						{
							is_space_free = false;
							break;
						}

						auto& new_tile = world_map(x + i, y - j);

						if (TileManager::get().getProperties(new_tile.id).is_solid)
						{
							is_space_free = false;
							break;
						}

					}

					if (!is_space_free) break;
				}

				if (!is_space_free) continue;

				//Fourth - Does the object intersect with any other object?

				//Calculate rect
				float y_offset = static_cast<float>(object_size.y) * tile_height_world;
				SDL_FRect object_rect
				{
					position_x * tile_width_world,
					position_y * tile_height_world - y_offset,
					//Subtracted -1 from their width and height because otherwise SDL_HasRectIntersectionFloat would return true if the edges of both object collide
					static_cast<float>(object_size.x) * tile_width_world - 1.f,
					static_cast<float>(object_size.y) * tile_height_world - 1.f
				};

				bool object_intersection = false;
				for (const auto& obj : objects)
				{
					if (SDL_HasRectIntersectionFloat(&object_rect, &obj.rect))
					{
						object_intersection = true;
						break;
					}
				}

				if (object_intersection) continue;

				/*//Fifth - object must completely fit inside a chunk that it belongs to

				float chunk_x_index = std::floor(object_rect.x / static_cast<float>(chunk_width_tiles));
				float chunk_y_index = std::floor(object_rect.y / static_cast<float>(chunk_height_tiles));
				SDL_FRect chunk_rect
				{ 
					chunk_x_index * tile_width_world,
					chunk_y_index * tile_height_world,
					static_cast<int>(chunk_width_tiles) * tile_width_world,
					static_cast<int>(chunk_height_tiles) * tile_height_world 
				};

				bool does_object_fit = isRectInsideFloat(object_rect, chunk_rect);

				if (!does_object_fit) continue;*/

				//FINALLY! Spawn an object
				int unique_id = next_object_id++;
				int properties_id = spawn_info.object_properties_id;
				objects.emplace_back(unique_id, properties_id, object_rect);
				break;
			}
		}
	}
}

void World::applyChanges()
{

}
