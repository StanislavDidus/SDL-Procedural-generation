#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "InputManager.hpp"

class ChangeMiningSizeSystem
{
public:
	ChangeMiningSizeSystem(entt::registry& registry) : registry{registry} {}

	void update()
	{
		if (!InputManager::isKey(SDL_SCANCODE_LCTRL)) return;

		const auto& mouse_state = InputManager::getMouseState();
		auto view = registry.view<Components::Player, Components::MiningAbility>();
		for (auto [entity,  mining_ability_component] : view.each())
		{
			std::cout << mouse_state.wheel.y << std::endl;

			if (mouse_state.wheel.y < 0.0f)
			{
				mining_ability_component.current_size--;
			}
			else if (mouse_state.wheel.y > 0.0f)
			{
				mining_ability_component.current_size++;
			}

			mining_ability_component.current_size = std::clamp(mining_ability_component.current_size, 1, mining_ability_component.max_size);
		}
	}

private:
	entt::registry& registry;
};
