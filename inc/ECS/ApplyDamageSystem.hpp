#pragma once

#include "ComponentManager.hpp"
#include "EnemySpawnSystem.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

class ApplyDamageSystem
{
public:
	ApplyDamageSystem() = default;

	void update(float dt)
	{
		auto& component_manager = ComponentManager::get();

		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (component_manager.damaged.contains(entity) && component_manager.health.contains(entity))
			{
				auto& damaged_components = component_manager.damaged.at(entity);
				auto& health_component = component_manager.health.at(entity);
				
				bool damage_taken = false;
				for (const auto& damage : damaged_components)
				{
					health_component.current_health -= damage.value;
					damage_taken = true;
				}

				if (damage_taken)
				{
					component_manager.hit_mark[entity] = HitMark{0.1f, 0.0f, false};	
				}

				damaged_components.clear();
			}
		}
	}
};
