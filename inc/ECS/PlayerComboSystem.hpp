#pragma once
#include "ComponentManager.hpp"
#include "EnemySpawnSystem.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

#include <queue>

class PlayerComboSystem
{
public:
	PlayerComboSystem(const std::shared_ptr<EnemySpawnSystem>& enemy_spawn_system) : enemy_spawn_system(enemy_spawn_system) {}

	void update(float dt, Entity target_entity)
	{
		auto enemy_spawn_system_s = enemy_spawn_system.lock();
		if (!enemy_spawn_system_s)
			return;

		auto& component_manager = ComponentManager::get();
		if (!component_manager.transform.contains(target_entity) || !component_manager.equipment.contains(target_entity)) return;

		const auto& player_transform_component = component_manager.transform.at(target_entity);
		auto& player_equipment_component = component_manager.equipment.at(target_entity);

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

					if (!component_manager.transform.contains(enemy) || !component_manager.health.contains(enemy)) continue;

					const auto& enemy_transform_component = component_manager.transform.at(enemy);
					auto& enemy_health = component_manager.health.at(enemy);

					float distance = glm::distance(enemy_transform_component.position, player_transform_component.position);

					if (distance > radius) continue;

					enemy_queue.emplace(distance, enemy);
				}

				if (!enemy_queue.empty())
				{
					const auto& [distance, enemy] = enemy_queue.top();

					if (component_manager.health.contains(enemy))
					{
						auto& enemy_health = component_manager.health.at(enemy);

						enemy_health.current_health -= damage;

						std::cout << "Deal damage" << std::endl;
					}
				}
			}
		}
	}

private:
	std::weak_ptr<EnemySpawnSystem> enemy_spawn_system;
};
