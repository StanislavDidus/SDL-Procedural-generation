#include "EnemySpawnSystem.hpp"

#include <ranges>

#include "EnemyManager.hpp"
#include "ResourceManager.hpp"
#include "ECS/ComponentManager.hpp"
#include "glm/gtc/random.hpp"
#include "Debug.hpp"

EnemySpawnSystem::EnemySpawnSystem(entt::registry& registry, const World& world, const SpawnRadius& spawn_radius)
	: world(world)
	, spawn_radius(spawn_radius)
	, registry(registry)
{
}

const std::vector<Entity>& EnemySpawnSystem::getEnemies() const
{
	return enemies;
}

void EnemySpawnSystem::update(float dt, const glm::vec2& target_position, graphics::Renderer& screen)
{
	time += dt;

	//std::cout << "x: " << target_position.x << " y: " << target_position.y << std::endl;

	//const auto& world_size_tiles = world.getWorldSizeTiles();
	const auto& tile_size = world.getTileSize();
	const auto& grid = world.getGrid();
	//const auto& camera_rect = graphics::getCameraRectFromTarget(screen, target_position);
	//float camera_half_width = camera_rect.w * 0.5f;

	if (time >= spawn_time)
	{//
		time = 0.0f;

		spawnEnemies(target_position, enemies_to_spawn);
	}

	{
		// Remove enemies that are far away
		auto begin = std::ranges::remove_if(enemies, [this, target_position](Entity enemy)
			{
				const auto& transform_component = registry.get<Components::Transform>(enemy);
				float distance = glm::distance(target_position, transform_component.position);
				bool destroy = distance > enemy_despawn_distance;
				if (destroy) destroyEnemy(enemy);
				return destroy;

			}).begin();

		enemies.erase(begin, enemies.end());
	}

	{
		// Remove enemies that have less than 0 health points
		auto begin = std::ranges::remove_if(enemies, [this, target_position](Entity enemy)
			{
				const auto& health_component = registry.get<Components::Health>(enemy);
				bool destroy = health_component.current_health <= 0.0f;
				if (destroy) destroyEnemy(enemy);
				return destroy;

			}).begin();

		enemies.erase(begin, enemies.end());
	}


#ifdef DEBUG_SPAWN_RADIUS
	graphics::drawCircle(screen, target_position.x, target_position.y, spawn_radius.spawn_minimum_radius, graphics::Color::GREEN);
	graphics::drawCircle(screen, target_position.x, target_position.y, spawn_radius.spawn_maximum_radius, graphics::Color::BLUE);
	graphics::drawCircle(screen, target_position.x, target_position.y, enemy_despawn_distance, graphics::Color::RED);
#endif
}

Entity EnemySpawnSystem::createEntity(size_t id) const
{
	auto entity = registry.create();

	const auto& enemy_data = EnemyManager::get().getEnemyData(id);

	auto& renderable = registry.emplace<Components::Renderable>(entity);
	renderable.sprite = (*ResourceManager::get().getSpriteSheet("enemies"))[enemy_data.sprite_index];

	auto& physics = registry.emplace<Components::Physics>(entity);
	physics.can_move_horizontal = true;
	physics.acceleration = glm::vec2{ 1000.0f, 0.0f };
	physics.max_velocity = glm::vec2{ 75.0f, 200.0f, };
	physics.decelaration = 5.0f;

	auto& physic_step = registry.emplace<Components::PhysicStep>(entity);
	physic_step.max_step_height = 20.0f;

	auto& jump = registry.emplace<Components::Jump>(entity);
	jump.jump_force = 425.0f;

	auto& health = registry.emplace<Components::Health>(entity);
	health.max_health = 100.0f;
	health.current_health = 100.0f;

	auto& enemy_ai = registry.emplace<Components::EnemyAI>(entity);
	enemy_ai.position_update_time = 2.0f;
	
	return entity;
}

void EnemySpawnSystem::destroyEnemy(Entity enemy) const
{
	registry.destroy(enemy);
}

void EnemySpawnSystem::spawnEnemies(const glm::vec2& target_position, int number)
{
	if (enemies.size() >= max_enemies) return;

	const auto& tile_size = world.getTileSize();
	const auto& grid = world.getGrid();

	std::unordered_map<size_t, std::vector<glm::ivec2>> possible_enemy_spawns; // size_t - enemy id and the position where the enemy can be spawned

	int i = 0;
	for (const auto& tile : grid.data())
	{
		int x = i % grid.getColumns();
		int y = i / grid.getColumns();

		glm::vec2 tile_global_position =
		{
			static_cast<float>(x) * tile_size.x,
			static_cast<float>(y) * tile_size.y
		};

		//Distance from a player to a tile
		float distance = glm::distance(tile_global_position, target_position);

		if (spawn_radius.spawn_minimum_radius < distance && distance < spawn_radius.spawn_maximum_radius)
		{
			const auto& spawn_infos = EnemyManager::get().getAllEnemySpawnInfo();

			//std::vector<EnemySpawnInfo> enemy_pool; // Enemies that potentially can spawn on this tile
			for (const auto& info : spawn_infos)
			{
				if (std::ranges::find(info.spawn_tiles, tile.id) != info.spawn_tiles.end())
				{
					auto& enemy_size = info.size;

					//Check if space above the tile is free
					bool is_space_free = true;
					for (int i = 0; i < enemy_size.x; ++i)
					{
						//Set a small 1 tile offset for y loop because we want to check tiles above the ground
						for (int j = 1; j <= enemy_size.y; ++j)
						{

							//Check the boundaries of thw world before getting a tile from the world_map
							int new_x = x + i;
							int new_y = y - j;

							if (new_x < 0 || new_x >= grid.getColumns() || new_y < 0 || new_y >= grid.getRows())
							{
								is_space_free = false;
								break;
							}

							auto& new_tile = grid(x + i, y - j);

							if (TileManager::get().getProperties(new_tile.id).is_solid)
							{
								is_space_free = false;
								break;
							}

						}

						if (!is_space_free) break;
					}

					if (!is_space_free)
					{
						continue;
					}

					possible_enemy_spawns[info.id].emplace_back(x, y - enemy_size.y);
				}
			}
		}

		++i;
	}

	float total_weight = 0.0f;
	for (const auto& id : possible_enemy_spawns | std::views::keys)
	{
		const auto& spawn_info = EnemyManager::get().getEnemySpawnInfo(id);
		total_weight += spawn_info.spawn_chance_weight;
	}

	for (int i = 0; i < number; ++i)
	{
		if (enemies.size() >= max_enemies) return;

		const float rng = glm::linearRand(0.0f, total_weight);
		float acc = 0.0f;
		for (const auto& [id, positions] : possible_enemy_spawns)
		{
			const auto& spawn_info = EnemyManager::get().getEnemySpawnInfo(id);
			acc += spawn_info.spawn_chance_weight;
			if (rng <= acc)
			{
				int rng_position = glm::linearRand(0, static_cast<int>(positions.size() - 1));

				const auto& position = positions[rng_position];

				//
				// Spawn enemy at position
				//

				auto enemy = createEntity(spawn_info.id);

				const auto& enemy_size = spawn_info.size;
				auto& ts = registry.emplace<Components::Transform>(enemy);
				ts.position = { tile_size.x * position.x, tile_size.y * position.y };
				ts.size = { tile_size.x * enemy_size.x, tile_size.y * enemy_size.y };

				enemies.push_back(enemy);

				break;
			}
		}
	}
}
