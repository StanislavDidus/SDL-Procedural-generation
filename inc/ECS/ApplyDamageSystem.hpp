#pragma once

#include "ComponentManager.hpp"
#include "Components.hpp"
#include "EnemySpawnSystem.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

class ApplyDamageSystem
{
public:
	ApplyDamageSystem(entt::registry& registry) : registry(registry) {}

	void update(float dt)
	{
		timer += dt;
		std::vector<Entity> to_destroy;
		auto view = registry.view<Components::Damage>();
		to_destroy.reserve(view.size());
		for (auto [entity, damage_component] : view.each())
		{
			// Delete "Damage" entity if the target is currently invincible to damage
			if (registry.all_of<Components::Invincible>(damage_component.target))
			{
				to_destroy.push_back(entity);
			}

			// Otherwise deal damage
			else if (registry.all_of<Components::Health>(damage_component.target))
			{
				// Decrease health points
				registry.get<Components::Health>(damage_component.target).current_health -= damage_component.value;

				// Place a hit mark
				registry.emplace_or_replace<Components::HitMark>(damage_component.target, 0.1f, 0.0f, false);
				
				// If target is a player, then we add invincibility to it after the hit
				if (registry.all_of<Components::Player>(damage_component.target))
					registry.emplace<Components::Invincible>(damage_component.target, 3.0f, 0.0f);

				// Destroy this entity
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
	float timer = 0.0f;
};
