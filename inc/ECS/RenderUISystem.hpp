#pragma once

#include "Color.hpp"
#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "Renderer.hpp"
#include "CraftingManager.hpp"
#include "ResourceManager.hpp"

// TODO: Add update function that is going to update all buttons positions separating the logic.
class RenderUISystem
{
public:
	RenderUISystem(ComponentManager& component_manager, const EntityManager& entity_manager,  const UISettings& ui_settings)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, ui_settings(ui_settings)
	{
		
	}

	static void SortCraftButtons(Entity target_entity, const std::vector<Entity>& buttons, std::vector<Entity>& sorted_buttons)
	{
		
	}

	void update(Entity target_entity)
	{
		
	}

	void render(Renderer& screen, Entity target_entity)
	{
		//Render Craft Buttons
		std::vector<Entity> buttons_craftable;
		std::vector<Entity> buttons_missing_resources;
		std::vector<Entity> buttons_recipe_locked;

		if (!component_manager.has_inventory.contains(target_entity)) return;

		const auto& inventory = component_manager.has_inventory.at(target_entity).inventory;

		//Loop through all craft buttons and split them in 3 arrays for rendering order
		for (const auto& entity : entity_manager.getEntities())
		{
			if (!component_manager.transform.contains(entity) || !component_manager.button.contains(entity) || !component_manager.craft_button.contains(entity)) continue;

			const auto& ts = component_manager.transform.at(entity);
			const auto& button_component = component_manager.button.at(entity);
			const auto& craft_button_component = component_manager.craft_button.at(entity);

			if (craft_button_component.is_available)
			{
				const auto& recipe = CraftingManager::get().getRecipe(craft_button_component.recipe_id);

				bool is_enough = true;
				for (const auto& required_item : recipe.required_items)
				{
					int required_item_number = required_item.stack_number;
					int item_number_entity_has = inventory->countItem(required_item.id);
					bool is_enough_resources = item_number_entity_has >= required_item_number;

					if (!is_enough_resources)
					{
						buttons_missing_resources.push_back(entity);
						is_enough = false;
						break;
					}
				}

				if (is_enough)
				{
					buttons_craftable.push_back(entity);
				}

			}
			else
			{
				buttons_recipe_locked.push_back(entity);
			}
		}

		float craft_button_position_x = screen.getWindowSize().x - ui_settings.craft_button_columns * ui_settings.craft_button_width;
		float craft_button_position_y = 0.0f;

		int i = 0;
		for (const auto& button : buttons_craftable)
		{
			auto& transform_component = component_manager.transform.at(button);
			const auto& craft_button_component = component_manager.craft_button.at(button);
			
			int x = i % ui_settings.craft_button_columns;
			int y = i / ui_settings.craft_button_columns;

			transform_component.position.x = craft_button_position_x + x * ui_settings.craft_button_width;
			transform_component.position.y = craft_button_position_y + y * ui_settings.craft_button_height;
			transform_component.size.x = ui_settings.craft_button_width;
			transform_component.size.y = ui_settings.craft_button_height;

			size_t item_id = CraftingManager::get().getRecipe(craft_button_component.recipe_id).item_id;

			screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("ui")[0], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, IGNORE_VIEW_ZOOM);
			screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("items")[item_id], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, IGNORE_VIEW_ZOOM);

			++i;
		}

		for (const auto& button : buttons_missing_resources)
		{
			auto& trasform_component = component_manager.transform.at(button);
			const auto& craft_button_component = component_manager.craft_button.at(button);

			int x = i % ui_settings.craft_button_columns;
			int y = i / ui_settings.craft_button_columns;

			trasform_component.position.x = craft_button_position_x + x * ui_settings.craft_button_width;
			trasform_component.position.y = craft_button_position_y + y * ui_settings.craft_button_height;
			trasform_component.size.x = ui_settings.craft_button_width;
			trasform_component.size.y = ui_settings.craft_button_height;

			size_t item_id = CraftingManager::get().getRecipe(craft_button_component.recipe_id).item_id;

			screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("ui")[0], trasform_component.position.x, trasform_component.position.y, trasform_component.size.x, trasform_component.size.y, IGNORE_VIEW_ZOOM);
			screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("items")[item_id], trasform_component.position.x, trasform_component.position.y, trasform_component.size.x, trasform_component.size.y, IGNORE_VIEW_ZOOM);
			screen.drawRectangle(trasform_component.position.x, trasform_component.position.y, trasform_component.size.x, trasform_component.size.y, RenderType::FILL, Color{0,0,0,125}, IGNORE_VIEW_ZOOM);

			++i;
		}

		for (const auto& button : buttons_recipe_locked)
		{
			auto& trasform_component = component_manager.transform.at(button);
			const auto& craft_button_component = component_manager.craft_button.at(button);

			int x = i % ui_settings.craft_button_columns;
			int y = i / ui_settings.craft_button_columns;

			trasform_component.position.x = craft_button_position_x + x * ui_settings.craft_button_width;
			trasform_component.position.y = craft_button_position_y + y * ui_settings.craft_button_height;
			trasform_component.size.x = ui_settings.craft_button_width;
			trasform_component.size.y = ui_settings.craft_button_height;

			screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("ui")[1], trasform_component.position.x, trasform_component.position.y, trasform_component.size.x, trasform_component.size.y, IGNORE_VIEW_ZOOM);

			++i;
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
	
	const UISettings& ui_settings;
};
