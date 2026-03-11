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
			// Delete "Damage" target if the target is currently invincible to damage
			if (registry.all_of<Components::Invincible>(damage_component.target))
			{
			}

			// Otherwise deal damage
			else if (registry.all_of<Components::Health>(damage_component.target))
			{
				// Decrease health points
				registry.get<Components::Health>(damage_component.target).current_health -= damage_component.value;

				// Slowdown enemy if weapon has Freeze effect
				if (registry.all_of<Components::WeaponEffects::Freeze>(damage_component.source))
				{
					const auto& freeze_component = registry.get<Components::WeaponEffects::Freeze>(damage_component.source);
					auto effect_entity = registry.create();
					registry.emplace<Components::AddEffect>(effect_entity, damage_component.target, damage_component.source);
					registry.emplace<Components::WeaponEffects::Freeze>(effect_entity, freeze_component);
				}
				// Deal damage every second
				if (registry.all_of<Components::WeaponEffects::Poison>(damage_component.source))
				{
					const auto& poison_component = registry.get<Components::WeaponEffects::Poison>(damage_component.source);
					auto effect_entity = registry.create();
					registry.emplace<Components::AddEffect>(effect_entity, damage_component.target, damage_component.source);
					registry.emplace<Components::WeaponEffects::Poison>(effect_entity, poison_component);
				}

				// Place a hit mark
				registry.emplace_or_replace<Components::HitMark>(damage_component.target, 0.1f, 0.0f, false);
				
				// If target is a player, then we add invincibility to it after the hit
				// Also the player gets pushed aside
				if (registry.all_of<Components::Player, Components::Physics>(damage_component.target) && registry.all_of<Components::Transform>(damage_component.source))
				{
					const auto& player_transform_position = registry.get<Components::Transform>(damage_component.target);
					const auto& enemy_transform_component = registry.get<Components::Transform>(damage_component.source);

					glm::vec2 player_mid_position = player_transform_position.position + player_transform_position.size * 0.5f;
					glm::vec2 enemy_mid_position = enemy_transform_component.position + enemy_transform_component.size * 0.5f;

					auto& player_physics_component = registry.get<Components::Physics>(damage_component.target);

					float direction = player_mid_position.x < enemy_mid_position.x ? -1.0f : 1.0f;
					float push_force_x = 600.0f;
					float push_force_y = -300.0f;
					player_physics_component.velocity.x = push_force_x * direction;
					player_physics_component.velocity.y = push_force_y;

					registry.emplace<Components::Invincible>(damage_component.target, 0.5f, 0.0f);


				}
			}
			to_destroy.emplace_back(entity);
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
