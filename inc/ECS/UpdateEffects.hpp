#pragma once

#include "Components.hpp"
#include <entt/entity/registry.hpp>

class UpdateEffects
{
public:
	UpdateEffects(entt::registry& registry) : registry{registry} {}

	void update(float dt)
	{
		std::vector<Entity> to_destroy;

		auto view = registry.view<Components::WeaponEffects::Effect, Components::WeaponEffects::EffectDuration>();
		for (auto [entity, effect_component, effect_duration_component] : view.each())
		{
			effect_duration_component.timer += dt;

			if (effect_duration_component.timer >= effect_duration_component.time)
			{
				to_destroy.push_back(entity);
			}
		}

		for (const auto& entity : to_destroy) registry.destroy(entity);
	}

private:
	entt::registry& registry;
};