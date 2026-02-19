#pragma once

#include "ComponentManager.hpp"
#include "EnemySpawnSystem.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

class ApplyDamageSystem
{
public:
	ApplyDamageSystem(entt::registry& registry) : registry(registry) {}

	void update(float dt)
	{
		auto view = registry.view<Components::Damaged, Components::Health>();
		for (auto [entity, damaged_components, health_component] : view.each());
		{
			/*
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
	*/
		}
	}

private:
	entt::registry& registry;
};
