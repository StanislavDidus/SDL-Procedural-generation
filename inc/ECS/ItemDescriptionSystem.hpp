#pragma once
#include "Color.hpp"
#include "ComponentManager.hpp"
#include "CraftingManager.hpp"
#include "EntityManager.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ItemManager.hpp"

constexpr float LABEL_WIDTH = 250.f;
constexpr float LABEL_HEIGHT = 125.f;

class ItemDescriptionSystem
{
public:
	ItemDescriptionSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	void render(Renderer& screen)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.button_covered.contains(entity) && component_manager.craft_button.contains(entity))
			{
				const auto& transform_component = component_manager.transform.at(entity);
				const auto& craft_button_component = component_manager.craft_button.at(entity);

				if (!craft_button_component.is_available) continue;
				
				const auto& recipe = CraftingManager::get().getRecipe(craft_button_component.recipe_id);
				size_t item_id = recipe.item_id;

				const auto& mouse_position = InputManager::getMouseState().position;

				glm::vec2 button_centre = transform_component.position + transform_component.size * 0.5f;

				float x = button_centre.x - LABEL_WIDTH * 0.5f;
				float y = transform_component.position.y + transform_component.size.y;

				screen.drawRectangle(x, y, LABEL_WIDTH, LABEL_HEIGHT, RenderType::FILL, Color{0,0,200,128}, IGNORE_VIEW_ZOOM);
				
				//Display some information about the item

				const auto& item = ItemManager::get().getItem(item_id);

				//Render text
			}
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
};
