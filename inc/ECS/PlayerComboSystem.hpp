#pragma once
#include "ComponentManager.hpp"
#include "EnemySpawnSystem.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

#include <queue>

class PlayerComboSystem
{
public:
	PlayerComboSystem(entt::registry& registry, const std::shared_ptr<EnemySpawnSystem>& enemy_spawn_system) : registry{ registry }, enemy_spawn_system(enemy_spawn_system) {}

	void update(float dt, Entity target_entity)
	{
		auto enemy_spawn_system_s = enemy_spawn_system.lock();
		if (!enemy_spawn_system_s)
			return;

		if (registry.all_of<Components::Transform, Components::Equipment>(target_entity))
		{
			auto& player_transform_component = registry.get<Components::Transform>(target_entity);
			auto& player_equipment_component = registry.get<Components::Equipment>(target_entity);

			for (auto& weapon : player_equipment_component.weapons)
			{
				if (!weapon) continue;
				auto& weapon_properties = ItemManager::get().getProperties(weapon->id);
				if (!weapon_properties.melee_weapon_data) continue;

				float damage = weapon_properties.melee_weapon_data->damage;
				float radius = weapon_properties.melee_weapon_data->radius;
				float cooldown = weapon_properties.melee_weapon_data->cooldown;
				float& cooldown_timer = weapon->cooldown_timer;
				cooldown_timer += dt;

				if (cooldown_timer >= cooldown)
				{
					cooldown_timer = 0.0f;

					//Find the closest enemy
					using EnemyEntry = std::pair<float, Entity>;
					std::priority_queue<EnemyEntry, std::vector<EnemyEntry>, std::greater<>> enemy_queue;

					for (const auto& enemy : enemy_spawn_system_s->getEnemies())
					{
						//if (enemy == target_entity) continue;

						if (registry.all_of<Components::Transform, Components::Health>(enemy))
						{
							auto& enemy_transform_component = registry.get<Components::Transform>(enemy);
							auto& enemy_health = registry.get<Components::Health>(enemy);

							float distance = glm::distance(enemy_transform_component.position, player_transform_component.position);

							if (distance > radius) continue;

							enemy_queue.emplace(distance, enemy);
						}
					}

					if (!enemy_queue.empty())
					{
						const auto& [distance, enemy] = enemy_queue.top();

						if (registry.all_of<Components::Health>(enemy))
						{
							auto& enemy_health = registry.get<Components::Health>(enemy);

							auto damage_entity = registry.create();
							registry.emplace<Components::Damage>(damage_entity, enemy, damage);
						}
					}
				}
			}
		}
	}

private:
	entt::registry& registry;
	std::weak_ptr<EnemySpawnSystem> enemy_spawn_system;
};
