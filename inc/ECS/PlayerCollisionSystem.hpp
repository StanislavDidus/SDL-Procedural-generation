#pragma once
#include <entt/entity/registry.hpp>

#include "Entity.hpp"
#include "Components.hpp"
#include "Systems.hpp" // Needed for AABB collision functions

class PlayerCollisionSystem
{
public:
	PlayerCollisionSystem(entt::registry& registry) : registry{registry} {}

	void update(Entity player)
	{
		if (!registry.all_of<Components::Transform, Components::Health>(player)) return;
		const auto& player_transform = registry.get<Components::Transform>(player);
		const auto& player_health_component = registry.get<Components::Health>(player);

		auto view = registry.view<Components::CollideDamage, Components::Transform>();
		for (const auto& [entity, collide_damage_component, enemy_transform_component] : view.each())
		{
			if (AABB(player_transform, enemy_transform_component))
			{
				auto damage_entity = registry.create();
				registry.emplace<Components::Damage>(damage_entity, player, entity, collide_damage_component.damage);
			}
		}
	}

private:
	entt::registry& registry;
};
