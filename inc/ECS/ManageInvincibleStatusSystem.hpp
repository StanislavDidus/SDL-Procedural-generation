#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"

class ManageInvincibleStatusSystem
{
public:
	ManageInvincibleStatusSystem(entt::registry& registry) : registry{registry} {}

	void update(float dt)
	{
		auto view = registry.view<Components::Invincible>();
		for (auto [entity, invincible_component] : view.each())
		{
			invincible_component.timer += dt;

			if (invincible_component.timer > invincible_component.time)
				registry.erase<Components::Invincible>(entity);
		}
	}

private:
	entt::registry& registry;
};