#include "CraftView.hpp"

#include <iostream>

CraftView::CraftView(Entity target_entity, EntityManager& entity_manager, ComponentManager& component_manager, int rows, int columns, float slot_size, const glm::vec2& position)
	: UIElement(position, {})
	, rows(rows)
	, columns(columns)
	, slot_size(slot_size)
	, entity_manager(entity_manager)
	, component_manager(component_manager)
	, target_entity(target_entity)
{

	for (int i = 0; const auto& j : component_manager.crafting_ability.at(target_entity).recipes_acquired)
	{
		int x = i % columns;
		int y = i / columns;

		auto button = entity_manager.createEntity();

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

			component_manager.craft_button[*button] = CraftButton
			{
				true,
				j
			};
		}

		++i;
	}

	size_t recipes_acquired_size = component_manager.crafting_ability.at(target_entity).recipes_acquired.size();
	size_t recipes_left = CraftingManager::get().size() - recipes_acquired_size;
	for (size_t i = recipes_acquired_size, j = 0; j < recipes_left; ++j, ++i)
	{
		int x = static_cast<int>(i) % columns;
		int y = static_cast<int>(i) / columns;

		auto button = entity_manager.createEntity();

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

			component_manager.craft_button[*button] = CraftButton
			{
				false
			};
		}
	}
}

void CraftView::update()
{

}

void CraftView::render(Renderer& screen)
{

}
