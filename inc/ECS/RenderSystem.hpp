#pragma once

#include "Color.hpp"
#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "Renderer.hpp"
#include "CraftingManager.hpp"
#include "ResourceManager.hpp"

class RenderSystem
{
public:
	RenderSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{
		
	}

	void render(Renderer& screen)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (!component_manager.transform.contains(entity)) continue;

			const auto& ts = component_manager.transform.at(entity);

			//Render Craft Buttons
			if (component_manager.button.contains(entity) && component_manager.craft_button.contains(entity))
			{
				const auto& button_component = component_manager.button.at(entity);
				const auto& craft_button_component = component_manager.craft_button.at(entity);

				if (craft_button_component.is_available)
				{
					size_t item_id = CraftingManager::get().getRecipe(craft_button_component.recipe_id).item_id;

					screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("ui")[0], ts.position.x, ts.position.y, ts.size.x, ts.size.y, IGNORE_VIEW_ZOOM);
					screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("items")[item_id], ts.position.x, ts.position.y, ts.size.x, ts.size.y, IGNORE_VIEW_ZOOM);
				}
				else
				{
					screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("ui")[1], ts.position.x, ts.position.y, ts.size.x, ts.size.y, IGNORE_VIEW_ZOOM);
				}
			}
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
};
