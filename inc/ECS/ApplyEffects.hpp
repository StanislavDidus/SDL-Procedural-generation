#pragma once

#include "Components.hpp"
#include <entt/entity/registry.hpp>

class ApplyEffects
{
public:
	ApplyEffects(entt::registry& registry) : registry{registry} {}
	
	void update(float dt)
	{
		std::vector<Entity> to_destroy;
		{
			auto view = registry.view<Components::ChangeSpeed>();
			for (const auto& [entity, change_speed_component] : view.each())
			{
				auto target = change_speed_component.target;
				if (registry.all_of<Components::Physics>(target) && registry.all_of<Components::BaseValues>(target))
				{
					auto& physics_component = registry.get<Components::Physics>(target);
					const auto& base_values_component = registry.get<Components::BaseValues>(target);
					float value = registry.get<Components::ChangeSpeed>(entity).value;

					glm::vec2 acceleration_to_add = base_values_component.acceleration * (value / 100.0f);
					glm::vec2 max_velocity_to_add = base_values_component.max_velocity * (value / 100.0f);
					physics_component.acceleration += acceleration_to_add;
					physics_component.max_velocity += max_velocity_to_add;

					to_destroy.push_back(entity);
				}
			}
		}
		{
			auto view = registry.view<Components::WeaponEffects::Poison, Components::EffectDuration>();
			for (auto [entity, poison_component, effect_duration_component] : view.each())
			{
				poison_component.timer += dt;

				if (poison_component.timer >= 1.0f)
				{
					poison_component.timer = 0.0f;
					auto damage_entity = registry.create();
					std::cout << "poison damage" << std::endl;
					registry.emplace<Components::Damage>(damage_entity, effect_duration_component.target, effect_duration_component.source, poison_component.value);
				}
			}
		}

		for (const auto& entity : to_destroy) registry.destroy(entity);
	}

private:
	entt::registry& registry;
};