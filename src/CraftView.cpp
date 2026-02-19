#include "CraftView.hpp"

#include <iostream>

using namespace graphics;

CraftView::CraftView(entt::registry& registry, Entity target_entity, int rows, int columns, float slot_size, const glm::vec2& position)
	: UIElement(position, {})
	, rows(rows)
	, columns(columns)
	, slot_size(slot_size)
	, target_entity(target_entity)
	, registry(registry)
{

	if (registry.all_of<Components::CraftingAbility>(target_entity))
	{
		for (size_t i = 0; i < CraftingManager::get().size(); ++i)
		{
			int x = i % columns;
			int y = i / columns;

			const auto& recipes_acquired = registry.get<Components::CraftingAbility>(target_entity).recipes_acquired;

			auto button = registry.create();
			auto& ts = registry.emplace<Components::Transform>(button);
			ts.position = glm::vec2{ position.x + static_cast<float>(x) * slot_size, position.y + static_cast<float>(y) * slot_size };
			ts.size = glm::vec2{ slot_size };

			registry.emplace<Components::Button>(button);
			
			bool is_recipe_available = std::ranges::find(recipes_acquired, i) != recipes_acquired.end();

			auto& craft_button = registry.emplace<Components::CraftButton>(button);
			craft_button.is_available = is_recipe_available;
			craft_button.recipe_id = i;
		}
	}
}

void CraftView::update()
{

}

void CraftView::render(graphics::Renderer& screen)
{

}
