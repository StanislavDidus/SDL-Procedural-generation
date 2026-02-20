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
		std::vector<Entity> to_destroy;
		auto view = registry.view<Components::Damage>();
		to_destroy.reserve(view.size());
		for (auto [entity, damage_component] : view.each())
		{
			if (registry.all_of<Components::Health>(damage_component.target))
			{
				registry.get<Components::Health>(damage_component.target).current_health -= damage_component.value;
				registry.emplace_or_replace<Components::HitMark>(damage_component.target, 0.1f, 0.0f, false);
				to_destroy.emplace_back(entity);
			}
		}

		for (const auto& entity : to_destroy)
		{
			registry.destroy(entity);
		}
	}

private:
	entt::registry& registry;
};
