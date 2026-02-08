#include "CraftView.hpp"

#include <iostream>

using namespace graphics;

CraftView::CraftView(Entity target_entity, int rows, int columns, float slot_size, const glm::vec2& position)
	: UIElement(position, {})
	, rows(rows)
	, columns(columns)
	, slot_size(slot_size)
	, target_entity(target_entity)
{
	auto& component_manager = ComponentManager::get();

	if (component_manager.crafting_ability.contains(target_entity))
	{
		for (size_t i = 0; i < CraftingManager::get().size(); ++i)
		{
			int x = i % columns;
			int y = i / columns;

			auto button = EntityManager::get().createEntity();

			const auto& recipes_acquired = component_manager.crafting_ability.at(target_entity).recipes_acquired;

			if (button)
			{
				component_manager.transform[*button] = Transform
				{
					glm::vec2{position.x + static_cast<float>(x) * slot_size, position.y + static_cast<float>(y) * slot_size},
					glm::vec2{slot_size}
				};

				component_manager.button[*button] = Button
				{

				};

				bool is_recipe_available = std::ranges::find(recipes_acquired, i) != recipes_acquired.end();

				component_manager.craft_button[*button] = CraftButton
				{
					is_recipe_available,
					i
				};
			}
		}
	}
}

void CraftView::update()
{

}

void CraftView::render(graphics::Renderer& screen)
{

}
