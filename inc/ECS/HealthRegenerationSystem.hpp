#pragma once

#include "Components.hpp"

class HealthRegenerationSystem
{
public:
	HealthRegenerationSystem(entt::registry& registry)
		: registry{registry} {}

	void update(float dt)
	{
		auto view = registry.view<Components::Health, Components::Regeneration>();
		for (auto [entity, health_component, regeneration_component] : view.each())
		{
			regeneration_component.timer += dt;

			if (regeneration_component.timer >= 1.0f)
			{
				health_component.current_health += regeneration_component.speed;
				health_component.current_health = std::min(health_component.max_health, health_component.current_health + regeneration_component.speed);
				regeneration_component.timer = 0.0f;
			}
		}
	}
private:
	entt::registry& registry;
};