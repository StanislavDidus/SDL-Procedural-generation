#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"

class ManageButtonActionsSystem
{
public:
	ManageButtonActionsSystem(entt::registry& registry) : registry{registry} {}

	void update(Entity target_entity, graphics::Renderer& screen)
	{
		auto view = registry.view<Components::ButtonReleased, Components::CraftButton>();
		for (const auto& [entity, craft_button_component] : view.each())
		{
			if (craft_button_component.is_available)
			{
				registry.emplace<Components::CraftItem>(entity, craft_button_component.recipe_id);
			}
		}

		auto view2 = registry.view<Components::ButtonReleased, Components::Chest, Components::Transform>(entt::exclude<Components::Closed_Chest>);
		for (const auto& [entity, chest_component, button_transform_component] : view2.each())
		{
			if (registry.all_of<Components::Transform>(target_entity))
			{
				const auto& player_transform_component = registry.get<Components::Transform>(target_entity);
				float distance = glm::distance(player_transform_component.position, button_transform_component.position);

				if (distance < 200.0f)
				{
					auto chest_window = registry.create();

					registry.emplace<Components::UI::OpenChestWindow>(chest_window, target_entity, entity);
				}
			}
		}

		auto view3 = registry.view<Components::ButtonReleased, Components::ButtonFunction>();
		for (auto [entity, button_function_component] : view3.each())
		{
			button_function_component.command();
		}
	}
private:
	entt::registry& registry;
};
