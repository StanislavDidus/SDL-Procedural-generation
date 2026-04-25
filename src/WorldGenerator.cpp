#include "WorldGenerator.hpp"
#include "Debug.hpp"
#include "ECS/Systems.hpp"

#include <random>
#include <ranges>

#include "ResourceManager.hpp"
#include "ViewportGuard.hpp"
#include "WorldHelper.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "VertexBuffer.hpp"

WorldGenerator::WorldGenerator
(
	GenerationData& generation_data,
	entt::registry& registry,
	int world_width_tiles,
	int world_height_tiles
)
	: generation_data(generation_data)
	  , registry(registry), master_seed(0)
	, world_width_tiles(world_width_tiles)
	, world_height_tiles(world_height_tiles)

{
}

std::shared_ptr<World> WorldGenerator::generateWorld(std::optional<int> seed)
{
	Grid<Tile> grid(world_height_tiles, world_width_tiles);

	//Remove all chests
	auto view = registry.view<Components::Chest>();
	std::vector<Entity> to_destroy;
	for (const auto& [entity, chest] : view.each()) to_destroy.emplace_back(entity);
	for (const auto& entity : to_destroy) registry.destroy(entity);

	initSeeds(seed);

	generateBase(grid);
	addGrass(grid);
	addDirt(grid);
	addCaves(grid);
	if (generation_data.use_cellular_automata)
		for (int i = 0; i < generation_data.cellular_automata_iterations; i++) applyCellularAutomata(grid);
	//addWater();
	addBiomes(grid);
	
	std::vector<PortalData> portals;
	addPortals(portals, grid);

	std::vector<ObjectData> objects;
	addObjects(objects, portals, grid);

	std::vector<ChestData> chests;
	addChests(chests, objects, portals, grid);

	setTileDurability(grid);

	return std::make_shared<World>( grid, portals, objects, chests );
}

void WorldGenerator::initSeeds(std::optional<int> seed_opt)
{
	std::random_device random_device;
	master_seed = seed_opt.value_or(random_device());

	std::mt19937 seed_rng(master_seed);
	for (auto& noise : generation_data.noise_settings | std::views::values)
	{
		noise.seed = seed_rng();
	}
}

void WorldGenerator::generateBase(Grid<Tile>& grid) const
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);

		float peaks_and_valleys = Noise::fractal1D<ValueNoise>(generation_data.noise_settings.at(NoiseType::PV), position_x * scale);

		float map_height = generation_data.maps.at(MapRangeType::PV_HEIGHT).getValue(peaks_and_valleys);

		float new_change = generation_data.maps.at(MapRangeType::PV_CHANGE).getValue(peaks_and_valleys);

		for (int y = 0; y < world_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

			float density_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings.at(NoiseType::DENSITY), position_x * scale, position_y * scale);

			float new_height = position_y - map_height;

			//Apply y base layer and chance over vertical axis
			float difference = new_height - generation_data.y_base;
			float change = difference * new_change;
			density_noise += change;

			auto& tile = grid(x, y);

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

void WorldGenerator::addGrass(Grid<Tile>& grid) const
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		bool was_air = false;
		for (int y = 0; y < world_height_tiles; y++)
		{
			auto& tile = grid(x, y);
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

void WorldGenerator::addDirt(Grid<Tile>& grid) const
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		bool found_surface = false;
		int surface_y = 0;

		float dirt_noise = Noise::fractal1D<ValueNoise>(generation_data.noise_settings.at(NoiseType::DIRT), static_cast<float>(x) * scale);
		//float dirt_noise = ValueNoise::noise1D(static_cast<float>(x) * scale * 0.2f, seeds[2]) * 1.f;
		int dirt_level = static_cast<int>(dirt_noise * 5.f) + 5;

		for (int y = 0; y < world_height_tiles; y++)
		{
			auto& tile = grid(x, y);
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

void WorldGenerator::addCaves(Grid<Tile>& grid) const
{
	float scale = generation_data.scale;

	//Two different cave generation algorithms
	if (!generation_data.use_new_cave_generation)
	{
		for (int x = 0; x < world_width_tiles; x++)
		{
			for (int y = 0; y < world_height_tiles; y++)
			{
				auto& tile = grid(x, y);
				bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

				float cave_noise = Noise::fractal2D<ValueNoise>(generation_data.noise_settings.at(NoiseType::CAVE), static_cast<float>(x) * scale, static_cast<float>(y) * scale);

				float c_difference = (generation_data.cave_y_base - static_cast<float>(y)) * -1.f;
				auto& change_map = generation_data.maps.at(MapRangeType::CAVE_CHANGE);
				float correlated_cave_threshold = std::clamp(change_map.getValue(c_difference), 0.001f, change_map.getOutMax());

				if (is_solid && cave_noise < correlated_cave_threshold)
				{
					removeTileCave({ x, y }, grid);
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
				auto& tile = grid(x, y);
				bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

				float noise_value = Noise::fractal2D<ValueNoise>(generation_data.noise_settings.at(NoiseType::DRUNK_WALKER), static_cast<float>(x) * scale, static_cast<float>(y) * scale);

				if (noise_value < generation_data.drunk_walker_threshold && is_solid)
				{
					drunk_walker_position.emplace_back(x, y);
				}
			}
		}

		int current_number = 0;

		size_t sky_tile_id = TileManager::get().getTileID("Sky");
		//std::cout << drunk_walker_position.size() << std::endl;
		while (current_number < generation_data.drunk_walker_target_number && !drunk_walker_position.empty())
		{
			std::uniform_int_distribution<int> pos_dist(0, drunk_walker_position.size() - 1);
			int index = pos_dist(rng);

			glm::ivec2 position = drunk_walker_position[index];

			drunk_walker_position.erase(drunk_walker_position.begin() + index);

			if (position.y < static_cast<int>(generation_data.cave_y_base)) continue;

			++current_number;

			int size = generation_data.maps.at(MapRangeType::CAVE_SIZE_CHANGE).getValue(position.y);

			DrunkWalker walker{ position, size };

			while (walker.isFinished() == false)
			{
				walker.move(rng() % 4);

				glm::ivec2 new_position = walker.getPosition();

				if (new_position.x < 0 || new_position.x + 1 >= world_width_tiles || new_position.y < 0 || new_position.y + 1 >= world_height_tiles)
					break;

				grid(new_position.x, new_position.y).id = sky_tile_id;
				grid(new_position.x + 1, new_position.y).id = sky_tile_id;
				grid(new_position.x, new_position.y + 1).id = sky_tile_id;
				grid(new_position.x + 1, new_position.y + 1).id = sky_tile_id;

				removeTileCave({ new_position.x, new_position.y }, grid);
				removeTileCave({ new_position.x + 1, new_position.y }, grid);
				removeTileCave({ new_position.x, new_position.y + 1 }, grid);
				removeTileCave({ new_position.x + 1, new_position.y + 1 }, grid);
			}
		}
	}
}

void WorldGenerator::removeTileCave(const glm::ivec2& position, Grid<Tile>& grid) const
{
	auto& tile = grid(position.x, position.y);
	tile.id = TileManager::get().getTileID("Sky");

	for (int i = -1; i < 1; ++i)
	{
		for (int j = -1; j < 1; ++j)
		{
			int new_x = position.x + i;
			int new_y = position.y + j;
			if (new_x < 0 || new_x >= world_width_tiles || new_y < 0 || new_y >= world_height_tiles) continue;
			grid(new_x, new_y).cave_tile = true;
		}
	}
}

void WorldGenerator::applyCellularAutomata(Grid<Tile>& grid)
{
	Grid<Tile> new_map = grid;
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
		{
			if (!grid(x, y).cave_tile) continue;

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

					auto& new_tile = grid(new_x, new_y);
					if (TileManager::get().getProperties(new_tile.id).is_solid) ++solid_count;
				}
			}

			if (solid_count > 4) new_map(x, y).id = TileManager::get().getTileID("Stone");
			else if (solid_count <= 4) new_map(x, y).id = TileManager::get().getTileID("Sky");
		}
	}

	grid = new_map;
}

void WorldGenerator::addWater(Grid<Tile>& grid) const
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
		{
			auto& tile = grid(x, y);
			bool is_solid = TileManager::get().getProperties(tile.id).is_solid;

			if (!is_solid && static_cast<float>(y) > generation_data.sea_y_base && !tile.sealed)
			{
				tile.id = TileManager::get().getTileID("Water");
			}
		}
	}
}

void WorldGenerator::addBiomes(Grid<Tile>& grid) const
{
	float scale = generation_data.scale;
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
		{
			float position_x = static_cast<float>(x);
			float position_y = static_cast<float>(y);

			auto& tile = grid(x, y);
			TileType type = TileManager::get().getProperties(tile.id).type;

			if (type != TileType::SURFACE && type != TileType::DIRT) continue;

			float peaks_and_valleys = Noise::fractal1D<ValueNoise>(generation_data.noise_settings.at(NoiseType::PV), position_x * scale);
			float temperature = Noise::fractal2D<ValueNoise>(generation_data.noise_settings.at(NoiseType::TEMPERATURE), position_x * scale, position_y * scale);
			float moisture = Noise::fractal2D<ValueNoise>(generation_data.noise_settings.at(NoiseType::MOISTURE), position_x * scale, position_y * scale);

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

void WorldGenerator::addPortals(std::vector<PortalData>& portals, Grid<Tile>& grid)
{
	int portal_width = 7;
	int portal_height = 5;
	
	glm::vec2 target_position {static_cast<float>(world_width_tiles) * 0.5f, 0.0f};
	
	std::vector<glm::vec2> potential_positions;
	
	for (int x = 0; x < world_width_tiles; x++)
	{
		float position_x = static_cast<float>(x);
		for (int y = 0; y < world_height_tiles; y++)
		{
			float position_y = static_cast<float>(y);

				bool check_floor = checkTileFloor(grid, x, y, portal_width, [&] (const Tile& tile)
				{
					return TileManager::get().getProperties(tile.id).is_solid && tile.id != TileManager::get().getTileID("Stone");
				});

				if (!check_floor) continue;

				//Object Size
				glm::vec2 object_size = {portal_width, portal_height};

				bool is_space_free = checkTileSpace(grid, x, y, object_size.x, object_size.y);

				if (!is_space_free) continue;

				potential_positions.emplace_back(position_x, position_y);

		}
	}
	
	int index = -1;
	float closest_distance = std::numeric_limits<float>::max();
	
	for (int i = 0; const auto& position : potential_positions)
	{
		float distance = glm::distance(position, target_position);
		
		if (distance < closest_distance)
		{
			closest_distance = distance;
			index = i;
		}
		
		++i;
	}
	
	if (index == -1)
	{
		throw std::runtime_error{std::format("Could not spawn the Portal.")};
	}
	
	const auto& position = potential_positions[index];
	portals.emplace_back(SDL_FRect(position.x, position.y - portal_height, portal_width, portal_height));
	
}

void WorldGenerator::addObjects(std::vector<ObjectData>& objects, const std::vector<PortalData>& portals, Grid<Tile>& grid)
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
				bool check_floor = checkTileFloor(grid, x, y, spawn_info.size_tiles.x, [&spawn_info] (const Tile& tile)
				{
						auto it = std::ranges::find(spawn_info.spawn_tile_ids, tile.id);
						return it != spawn_info.spawn_tile_ids.end();
				});

				if (!check_floor) continue;

				//Object Size
				auto& object_size = spawn_info.size_tiles;

				bool is_space_free = checkTileSpace(grid, x, y, object_size.x, object_size.y);

				if (!is_space_free) continue;

				//Check if object intersects with any other object

				SDL_Rect object_grid_rect
				{
					x,
					y - object_size.y,
					object_size.x,
					object_size.y
				};

				bool intersect = false;
				for (const auto& object : objects)
				{
					if (object_grid_rect.x <= object.grid_rect.x + object.grid_rect.w && object_grid_rect.x + object_grid_rect.w >= object.grid_rect.x &&
					object_grid_rect.y <= object.grid_rect.y + object.grid_rect.h && object_grid_rect.y + object_grid_rect.h >= object.grid_rect.y )
					{
						intersect = true;
						break;
					}
				}

				if (intersect) continue;
				
				// Check if object intersects with portals
				bool intersect1 = false;
				
				for (const auto& portal : portals)
				{
					
					if (object_grid_rect.x <= portal.grid_rect.x + portal.grid_rect.w && object_grid_rect.x + object_grid_rect.w >= portal.grid_rect.x &&
					object_grid_rect.y <= portal.grid_rect.y + portal.grid_rect.h && object_grid_rect.y + object_grid_rect.h >= portal.grid_rect.y )
					{
						intersect1 = true;
						break;
					}
				}
				
				if (intersect1) continue;

				potential_objects_to_spawn.push_back(spawn_info);
			}

			float none_weight = 40.0f;
			float total_weight = none_weight;
			for (const auto& spawn_info : potential_objects_to_spawn)
			{
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
					//FINALLY! Calculate grid_rect

					auto& object_size = spawn_info.size_tiles;

					SDL_Rect object_grid_rect
					{
						x,
						y - object_size.y,
						object_size.x,
						object_size.y
					};

					int properties_id = spawn_info.object_properties_id;
					objects.emplace_back(object_grid_rect, properties_id);

					break;
				}
			}
		}
	}
}

void WorldGenerator::addChests(std::vector<ChestData>& chests, const std::vector<ObjectData>& objects, const std::vector<PortalData>& portals, Grid<Tile>& grid)
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

			//Check all 3 conditions

			//First - Threshold
			float random_value = static_cast<float>(chest_dist(rng)) / 100.0f;
			if (random_value > generation_data.chest_threshold) continue;

			auto& object_size = generation_data.chest_size;

			//Second - Tiles beneath must be solid
			bool check_floor = checkTileFloor(grid, x, y, object_size.x, [](const Tile& tile)
				{
					const auto& tile_properties = TileManager::get().getProperties(tile.id);
					return tile_properties.is_solid;
				});

			if (!check_floor) continue;

			//Third - Object Size
			bool is_space_free = checkTileSpace(grid, x, y, object_size.x, object_size.y);

			if (!is_space_free) continue;

			// Spawn an object

			SDL_Rect chest_grid_rect
			{
				x,
				y - generation_data.chest_size.y,
				generation_data.chest_size.x,
				generation_data.chest_size.y
			};
			
			// Check if chest intersects with other chests
			{
				bool intersect = false;
				for (const auto& chest : chests)
				{
					if (chest_grid_rect.x <= chest.grid_rect.x + chest.grid_rect.w && chest_grid_rect.x + chest_grid_rect.w >= chest.grid_rect.x &&
					chest_grid_rect.y <= chest.grid_rect.y + chest.grid_rect.h && chest_grid_rect.y + chest_grid_rect.h >= chest.grid_rect.y )
					{
						intersect = true;
						break;
					}
				}
				if (intersect) continue;
			}
			
			// Check if chest intersects with other objects
			{
				bool intersect = false;
				for (const auto& object : objects)
				{
					if (chest_grid_rect.x <= object.grid_rect.x + object.grid_rect.w && chest_grid_rect.x + chest_grid_rect.w >= object.grid_rect.x &&
					chest_grid_rect.y <= object.grid_rect.y + object.grid_rect.h && chest_grid_rect.y + chest_grid_rect.h >= object.grid_rect.y )
					{
						intersect = true;
						break;
					}
				}

				if (intersect) continue;
			}
			
			// Check if chest intersects with portals
			{
				bool intersect = false;
				for (const auto& portal : portals)
				{
					if (chest_grid_rect.x <= portal.grid_rect.x + portal.grid_rect.w && chest_grid_rect.x + chest_grid_rect.w >= portal.grid_rect.x &&
					chest_grid_rect.y <= portal.grid_rect.y + portal.grid_rect.h && chest_grid_rect.y + chest_grid_rect.h >= portal.grid_rect.y )
					{
						intersect = true;
						break;
					}
				}

				if (intersect) continue;
			}

			ChestData chest_data;
			chest_data.grid_rect = chest_grid_rect;

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
				float item_noise_value = Noise::fractal2D<ValueNoise>(generation_data.noise_settings.at(NoiseType::LOOT), scale * position_x, scale * position_y);
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
					chest_data.base_item = item_id;
					break;
				case LootType::COMMON:
					chest_data.common_item = item_id;
					break;
				case LootType::SNOW:
					chest_data.snow_item = item_id;
					break;
				case LootType::SAND:
					chest_data.sand_item = item_id;
					break;
				}
			}

			chests.push_back(chest_data);

			break;
		}
	}
}

void WorldGenerator::setTileDurability(Grid<Tile>& grid) const
{
	for (int x = 0; x < world_width_tiles; x++)
	{
		for (int y = 0; y < world_height_tiles; y++)
		{
			auto& tile = grid(x, y);
			tile.current_durability = TileManager::get().getProperties(tile.id).max_durability;
		}
	}
}
