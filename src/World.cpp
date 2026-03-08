#include "World.hpp"
#include "Debug.hpp"
#include "ECS/Systems.hpp"

#include <random>
#include <ranges>

#include "ResourceManager.hpp"
#include "WorldHelper.hpp"

namespace
{
	std::random_device rd;
	std::minstd_rand rng(rd());
}


World::World
(
	const GenerationData& generation_data,
	entt::registry& registry,
	std::shared_ptr<TileCollisionSystem> collision_system,
	int width_tiles, int height_tiles,
	float tile_width_world, float tile_height_world
)
	: generation_data(generation_data)
	, registry(registry)
	, world_map(height_tiles, width_tiles)
	, world_width_tiles(width_tiles)
	, world_height_tiles(height_tiles)
	, collision_system(collision_system)
	, tile_width_world(tile_width_world)
	, tile_height_world(tile_height_world)
	, world_width_chunks(static_cast<float>(width_tiles) / chunk_width_tiles)
	, world_height_chunks(static_cast<float>(height_tiles) / chunk_height_tiles)
{

}

const Grid<Tile>& World::getGrid() const
{
	return world_map;
}

glm::ivec2 World::getWorldSizeTiles() const
{
	return { world_width_tiles, world_height_tiles };
}

glm::vec2 World::getTileSize() const
{
	return { tile_width_world, tile_height_world };
}

void World::setCollisionSystem(std::shared_ptr<TileCollisionSystem> collision_system)
{
	this->collision_system = collision_system;
}

void World::update(const graphics::Renderer& screen, float dt, const glm::vec2& target)
{
	SDL_FRect rect = graphics::getCameraRectFromTarget(screen, target);
	camera_rect = rect;
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
			for (int x = 0; x < chunk_width_tiles; ++x)
			{
				for (int y = 0; y < chunk_height_tiles; ++y)
				{
					//auto& tile = chunk.tilemap(x, y);
					auto& tile = world_map(chunk.rect.x / tile_width_world + x, chunk.rect.y / tile_height_world + y);
					const auto& tile_properties = getTileProperties(tile.id);

					SDL_FRect tile_rect;
					tile_rect.x = chunk.rect.x + x * 20.f;
					tile_rect.y = chunk.rect.y + y * 20.f;
					tile_rect.w = 20.f;
					tile_rect.h = 20.f;

					if (SDL_HasRectIntersectionFloat(&camera_rect, &tile_rect))
					{
						int sprite_index = getTileProperties(tile.id).sprite_index;
						int is_solid = getTileProperties(tile.id).is_solid;

						//if (tile.current_durability == tile_properties.max_durability)
						graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("tiles"))[sprite_index], tile_rect.x, tile_rect.y, 20.f, 20.f);

						//Render break animation on top
						float durability_percentage = tile.current_durability / tile_properties.max_durability;
						const auto& break_sprite = *ResourceManager::get().getSpriteSheet("tile_break_anim");
						if (durability_percentage < 0.10f)
						{
							graphics::drawScaledSprite(screen, break_sprite[3], tile_rect.x, tile_rect.y, 20.f, 20.f);
						}
						else if (durability_percentage < 0.25f)
						{
							graphics::drawScaledSprite(screen, break_sprite[2], tile_rect.x, tile_rect.y, 20.f, 20.f);
						}
						else if (durability_percentage < 0.50f)
						{
							graphics::drawScaledSprite(screen, break_sprite[1], tile_rect.x, tile_rect.y, 20.f, 20.f);
						}
						else if (durability_percentage < 0.75f)
						{
							graphics::drawScaledSprite(screen, break_sprite[0], tile_rect.x, tile_rect.y, 20.f, 20.f);
						}

						//Add collisions
						if (auto s = collision_system.lock())
						{
							//if (is_solid) s->collisions.emplace_back(glm::ivec2{ static_cast<int>(tile_rect.x), static_cast<int>(tile_rect.y) }, glm::ivec2{ 20.f, 20.f });
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
					graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("objects"))[sprite_index], object.rect.x, object.rect.y, object.rect.w, object.rect.h);
				}
			}
		}
	}
}

void World::renderHelp(graphics::Renderer& screen) const
{
	// Render cave level
	{
		glm::vec2 position = { 0.0f, generation_data.cave_y_base * tile_height_world };
		glm::vec2 size = {world_width_tiles * tile_width_world, 0.0f};

		graphics::drawLine(screen, position.x, position.y, position.x + size.x, position.y + size.y, graphics::Color::RED);
		graphics::drawLine(screen, position.x, 1000.0f, position.x + size.x, 1000.0f, graphics::Color::RED);
	}
}

void World::placeTile(int tile_x, int tile_y, size_t tile_id)
{
	if (tile_x < 0 || tile_x > world_width_tiles - 1 | tile_y < 0 || tile_y > world_height_tiles - 1) return;

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

			if (new_x < 0 || new_x >= world_width_tiles || new_y < 0 || new_y >= world_height_tiles) continue;

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
	if (tile_x < 0 || tile_x > world_width_tiles - 1 | tile_y < 0 || tile_y > world_height_tiles - 1) return;

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

	auto& chunk = chunks[static_cast<size_t>(static_cast<float>(chunk_y_index) + static_cast<float>(chunk_x_index) * world_height_chunks)];

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
	int chunk_x_index = static_cast<int>(std::floor(mouse_global_position.x / (chunk_width_tiles * 20.f)));
	int chunk_y_index = static_cast<int>(std::floor(mouse_global_position.y / (chunk_height_tiles * 20.f)));

	int height_chunks = world_height_tiles / chunk_height_tiles;
	int width_chunks = world_width_tiles / chunk_width_tiles;

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

	//Remove all chests
	auto view = registry.view<Components::Chest>();
	std::vector<Entity> to_destroy;
	for (const auto& [entity, chest] : view.each()) to_destroy.emplace_back(entity);
	for (const auto& entity : to_destroy) registry.destroy(entity);


	initSeeds(seed);

	//tiles.resize(world_width_tiles * world_height_tiles);
	//chunks.resize(world_width_tiles / chunk_width_tiles * world_height_tiles / chunk_height_tiles);

	generateBase();
	addGrass();
	addDirt();
	addCaves();
	if (use_cellular_automata)
		for (int i = 0; i < 2; i++) applyCellularAutomata();
	//addWater();
	addBiomes();

	std::vector<Object> objects;
	addObjects(objects);

	std::vector<Entity> chests;
	addChests(objects, chests);

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
		chunks.emplace_back(SDL_FRect{}, glm::ivec2{});
	}
	/*
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
	}*/

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

		chunk.grid_position.x = chunk_x;
		chunk.grid_position.y = chunk_y;

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

	master_seed = seed;

	for (auto& noise : generation_data.noise_settings | std::views::values)
	{
		noise.seed = getNewSeed(seed);
	}

	/*
	for (auto& object : ObjectManager::get().getAllObjectSpawnInfos())
	{

	}
	*/

}

uint32_t World::getNewSeed(uint32_t master_seed)
{
	return master_seed + static_cast<uint32_t>(seeds_count++);	
}

void World::generateBase()
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		int tile_x = (x % chunk_width_tiles + chunk_width_tiles) % chunk_width_tiles;

		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(generation_data.noise_settings[NoiseType::PV], position_x * scale);

		float map_height = generation_data.maps[MapRangeType::PV_HEIGHT].getValue(peaks_and_valleys);

		float new_change = generation_data.maps[MapRangeType::PV_CHANGE].getValue(peaks_and_valleys);

		for (int y = 0; y < world_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			float density_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::DENSITY], position_x * scale, position_y * scale);

			float new_height = position_y - map_height;

			//Apply y base layer and chance over vertical axis
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
	for (int x = 0; x < world_width_tiles; x++)
	{
		bool was_air = false;
		for (int y = 0; y < world_height_tiles; y++)
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
	for (int x = 0; x < world_width_tiles; x++)
	{
		bool found_surface = false;
		int surface_y = 0;

		float dirt_noise = Noise::fractal1D<ValueNoise>(generation_data.noise_settings[NoiseType::DIRT], static_cast<float>(x) * scale);
		//float dirt_noise = ValueNoise::noise1D(static_cast<float>(x) * scale * 0.2f, seeds[2]) * 1.f;
		int dirt_level = static_cast<int>(dirt_noise * 5.f) + 5;

		for (int y = 0; y < world_height_tiles; y++)
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

	//Two different cave generation algorithms
	if (!use_new_cave_generation)
	{
		for (int x = 0; x < world_width_tiles; x++)
		{
			for (int y = 0; y < world_height_tiles; y++)
			{
				auto& tile = world_map(x, y);
				bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

				float cave_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::CAVE], static_cast<float>(x) * scale, static_cast<float>(y) * scale);

				float c_difference = (generation_data.cave_y_base - static_cast<float>(y)) * -1.f;
				auto& change_map = generation_data.maps[MapRangeType::CAVE_CHANGE];
				float correlated_cave_threshold = std::clamp(change_map.getValue(c_difference), 0.001f, change_map.getOutMax());

				if (is_solid && cave_noise < correlated_cave_threshold)
				{
					removeTileCave({ x, y });
				}
			}
		}
	}
	else
	{
		std::mt19937 rng(master_seed);

		std::vector<glm::ivec2> drunk_walker_position;
		for (int x = 0; x < world_width_tiles; x++)
		{
			for (int y = 0; y < world_height_tiles; y++)
			{
				auto& tile = world_map(x, y);
				bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

				float noise_value = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::DRUNK_WALKER], static_cast<float>(x) * scale, static_cast<float>(y) * scale);

				if (noise_value < generation_data.drunk_walker_threshold && is_solid)
				{
					drunk_walker_position.emplace_back(x, y);
				}
			}
		}

		int target_number = 50;
		int current_number = 0;

		size_t sky_tile_id = TileManager::get().getTileID("Sky");
		std::cout << drunk_walker_position.size() << std::endl;
		while (current_number < target_number && !drunk_walker_position.empty())
		{
			std::uniform_int_distribution<int> pos_dist(0, drunk_walker_position.size() - 1);
			int index = pos_dist(rng);

			glm::ivec2 position = drunk_walker_position[index];

			drunk_walker_position.erase(drunk_walker_position.begin() + index);

			if (position.y < static_cast<int>(generation_data.cave_y_base)) continue;

			++current_number;

			int size = generation_data.maps[MapRangeType::CAVE_SIZE_CHANGE].getValue(position.y);

			DrunkWalker walker{ position, size };

			while (walker.isFinished() == false)
			{
				walker.move(rng() % 4);

				glm::ivec2 new_position = walker.getPosition();

				if (new_position.x < 0 || new_position.x + 1 >= world_width_tiles || new_position.y < 0 || new_position.y + 1 >= world_height_tiles)
					break;

				world_map(new_position.x, new_position.y).id = sky_tile_id;
				world_map(new_position.x + 1, new_position.y).id = sky_tile_id;
				world_map(new_position.x, new_position.y + 1).id = sky_tile_id;
				world_map(new_position.x + 1, new_position.y + 1).id = sky_tile_id;

				removeTileCave({ new_position.x, new_position.y });
				removeTileCave({ new_position.x + 1, new_position.y });
				removeTileCave({ new_position.x, new_position.y + 1 });
				removeTileCave({ new_position.x + 1, new_position.y + 1 });
			}
		}
	}
}

void World::removeTileCave(const glm::ivec2& position)
{
	auto& tile = world_map(position.x, position.y);
	tile.id = TileManager::get().getTileID("Sky");

	for (int i = -1; i < 1; ++i)
	{
		for (int j = -1; j < 1; ++j)
		{
			int new_x = position.x + i;
			int new_y = position.y + j;
			if (new_x < 0 || new_x >= world_width_tiles || new_y < 0 || new_y >= world_height_tiles) continue;
			world_map(new_x, new_y).cave_tile = true;
		}
	}
}

void World::applyCellularAutomata()
{
	Grid<Tile> new_map = world_map;
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
		{
			if (!world_map(x, y).cave_tile) continue;

			int solid_count = 0;
			for (int i = -1; i <= 1; ++i)
			{
				for (int j = -1; j <= 1; ++j)
				{
					if (i == 0 && j == 0) continue;

					int new_x = x + i;
					int new_y = y + j;
					if (new_x < 0 || new_x >= world_width_tiles || new_y < 0 || new_y >= world_height_tiles)
					{
						++solid_count;
						continue;
					}

					auto& new_tile = world_map(new_x, new_y);
					if (TileManager::get().getProperties(new_tile.id).is_solid) ++solid_count;
				}
			}

			if (solid_count > 4) new_map(x, y).id = TileManager::get().getTileID("Stone");
			else if (solid_count <= 4) new_map(x, y).id = TileManager::get().getTileID("Sky");
		}
	}

	world_map = new_map;
}

void World::addWater()
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
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
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
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
	std::mt19937 rng(master_seed);
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		for (int y = 0; y < world_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			const auto& object_spawn_infos = ObjectManager::get().getAllObjectSpawnInfos();

			std::vector<ObjectSpawnInfo> potential_objects_to_spawn;

			for (const auto& spawn_info : object_spawn_infos)
			{
				bool check_floor = checkTileFloor(world_map, x, y, spawn_info.size_tiles.x, [&spawn_info] (const Tile& tile)
				{
						auto it = std::ranges::find(spawn_info.spawn_tile_ids, tile.id);
						return it != spawn_info.spawn_tile_ids.end();
				});

				if (!check_floor) continue;

				//Third - Object Size
				auto& object_size = spawn_info.size_tiles;

				bool is_space_free = checkTileSpace(world_map, x, y, object_size.x, object_size.y);

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

				potential_objects_to_spawn.push_back(spawn_info);
			}

			float none_weight = 40.0f;
			float total_weight = none_weight;
			for (const auto& spawn_info : potential_objects_to_spawn)
			{
				//TODO: Rename noise_threshold to spawn_weight
				total_weight += y < generation_data.cave_y_base ? spawn_info.spawn_weight : spawn_info.spawn_weight * 1.5f;
			}

			std::uniform_real_distribution<float> object_dist(0.0f, total_weight);
			float random_object_value = object_dist(rng);
			
			if (random_object_value < none_weight)
			{
				continue;
			}
			random_object_value -= none_weight;

			float acc = 0.0f;
			for (const auto& spawn_info : potential_objects_to_spawn)
			{
				acc += y < generation_data.cave_y_base ? spawn_info.spawn_weight : spawn_info.spawn_weight * 1.5f;
				if (random_object_value <= acc)
				{	
					//FINALLY! Spawn an object	SDL_FRect object_rect

					auto& object_size = spawn_info.size_tiles;
					float y_offset = static_cast<float>(object_size.y) * tile_height_world;
					
					SDL_FRect object_rect
					{
						position_x * tile_width_world,
						position_y * tile_height_world - y_offset,
						//Subtracted -1 from their width and height because otherwise SDL_HasRectIntersectionFloat would return true if the edges of both object collide
						static_cast<float>(object_size.x) * tile_width_world - 1.f,
						static_cast<float>(object_size.y) * tile_height_world - 1.f
					};

					int unique_id = next_object_id++;
					int properties_id = spawn_info.object_properties_id;
					objects.emplace_back(unique_id, properties_id, object_rect);

					break;
				}
			}

			/*
			float total_weight = 0.0f;
			for (const auto& spawn_info : object_spawn_infos)
			{
				total_weight += spawn_info.spawn_weight;
			}

			float noise_value = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::OBJECTS], position_x * scale, position_y * scale);
			float acc = 0.0f;
			const ObjectSpawnInfo* object_spawn_info = nullptr;
			for (const auto& spawn_info_ : object_spawn_infos)
			{
				acc += spawn_info_.spawn_weight;
				if (noise_value <= acc)
				{
					object_spawn_info = &spawn_info_;
					break;
				}
			}

			if (object_spawn_info == nullptr) continue;

			const auto& spawn_info = *object_spawn_info;
			//Check all 4 conditions

			//First - Noise

			//Second - Tile type
			bool check_floor = checkTileFloor(world_map, x, y, spawn_info.size_tiles.x, [&spawn_info] (const Tile& tile)
			{
					auto it = std::ranges::find(spawn_info.spawn_tile_ids, tile.id);
					return it != spawn_info.spawn_tile_ids.end();
			});

			if (!check_floor) continue;

			//Third - Object Size
			auto& object_size = spawn_info.size_tiles;

			bool is_space_free = checkTileSpace(world_map, x, y, object_size.x, object_size.y);

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

			//FINALLY! Spawn an object
			int unique_id = next_object_id++;
			int properties_id = spawn_info.object_properties_id;
			objects.emplace_back(unique_id, properties_id, object_rect);
			break;

			for (const auto& spawn_info : object_spawn_infos)
			{
				//Check all 4 conditions

				//First - Noise
				float noise_value = Noise::fractal2D<ValueNoise>(spawn_info.noise_settings, position_x * scale, position_y * scale);
				if (y > generation_data.cave_y_base) noise_value *= 0.5f;
				if (noise_value > spawn_info.spawn_weight) continue;

				//Second - Tile type
				bool check_floor = checkTileFloor(world_map, x, y, spawn_info.size_tiles.x, [&spawn_info] (const Tile& tile)
				{
						auto it = std::ranges::find(spawn_info.spawn_tile_ids, tile.id);
						return it != spawn_info.spawn_tile_ids.end();
				});

				if (!check_floor) continue;

				//Third - Object Size
				auto& object_size = spawn_info.size_tiles;

				bool is_space_free = checkTileSpace(world_map, x, y, object_size.x, object_size.y);

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

				//FINALLY! Spawn an object
				int unique_id = next_object_id++;
				int properties_id = spawn_info.object_properties_id;
				objects.emplace_back(unique_id, properties_id, object_rect);
				break;
			}
		*/
		}
	}
}

void World::addChests(const std::vector<Object>& objects, std::vector<Entity>& chests)
{
	std::mt19937 rng(master_seed);
	std::uniform_int_distribution<int> chest_dist (0,100);

	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		for (int y = 0; y < world_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			//Check all 5 conditions

			//First - Threshold
			float random_value = static_cast<float>(chest_dist(rng)) / 100.0f;
			if (random_value > generation_data.chest_threshold) continue;

			auto& object_size = generation_data.chest_size;

			//Second - Tiles beneath must be solid
			bool check_floor = checkTileFloor(world_map, x, y, object_size.x, [](const Tile& tile)
				{
					const auto& tile_properties = TileManager::get().getProperties(tile.id);
					return tile_properties.is_solid;
				});

			if (!check_floor) continue;

			//Third - Object Size
			bool is_space_free = checkTileSpace(world_map, x, y, object_size.x, object_size.y);

			if (!is_space_free) continue;

			//Forth - Does the chest intersect with any other object?

			//Calculate rect
			float y_offset = static_cast<float>(object_size.y) * tile_height_world;
			SDL_FRect chest_rect
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
				if (SDL_HasRectIntersectionFloat(&chest_rect, &obj.rect))
				{
					object_intersection = true;
					break;
				}
			}

			if (object_intersection) continue;

			// Fifth - Check if the chest intersects with any other chests

			bool chest_intersection = false;
			for (const auto& chest_ent : chests)
			{
				const auto& chest_transform_component = registry.get<Components::Transform>(chest_ent);
				SDL_FRect chest_ent_rect = {chest_transform_component.position.x, chest_transform_component.position.y, chest_transform_component.size.x, chest_transform_component.size.y};

				if (SDL_HasRectIntersectionFloat(&chest_rect, &chest_ent_rect))
				{
					chest_intersection = true;
					break;
				}
			}

			if (chest_intersection) continue;

			//FINALLY! Spawn an object	
			auto chest = registry.create();
			auto& ts = registry.emplace<Components::Transform>(chest);
			ts.position = { chest_rect.x, chest_rect.y };
			ts.size = { chest_rect.w, chest_rect.h };

			auto& renderable = registry.emplace<Components::Renderable>(chest);
			renderable.sprite = (*ResourceManager::get().getSpriteSheet("objects"))[8];
			
			auto& chest_component = registry.emplace<Components::Chest>(chest);

			// Set a random item to the chest
			//Calculate total weight of all items
			for (const auto& [chest_type, chest_loot] : generation_data.chest_loot)
			{
				float total_weight = 0.0f;
				for (const auto& item : chest_loot)
				{
					total_weight += item.drop_chance;
				}
				//Generate random value
				float item_noise_value = Noise::fractal2D<ValueNoise>(generation_data.noise_settings[NoiseType::LOOT], scale * position_x, scale * position_y);
				float random_number = item_noise_value * total_weight;
				//Find an item by using generated random value
				size_t item_id = chest_loot.back().item_id;
				float acc = 0.0f;
				for (const auto& item : chest_loot)
				{
					acc += item.drop_chance;
					if (random_number <= acc)
					{
						item_id = item.item_id;
						break;
					}
				}


				switch (chest_type)
				{
				case LootType::BASE:
					chest_component.base_item = item_id;
					break;
				case LootType::COMMON:
					chest_component.common_item = item_id;
					break;
				case LootType::SNOW:
					chest_component.snow_item = item_id;
					break;
				case LootType::SAND:
					chest_component.sand_item = item_id;
					break;
				}
			}

			registry.emplace<Components::Button>(chest, true);

			chests.push_back(chest);

			break;
		}
	}
}
