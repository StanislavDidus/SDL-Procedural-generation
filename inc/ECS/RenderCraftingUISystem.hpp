#pragma once

#include "Color.hpp"
#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"
#include "CraftingManager.hpp"
#include "ResourceManager.hpp"

// TODO: Add update function that is going to update all buttons positions separating the logic.
class RenderCraftingUISystem
{
public:
	RenderCraftingUISystem(entt::registry& registry, const UISettings& ui_settings)
		: ui_settings(ui_settings)
		, registry{registry}
	{
		
	}

	static void SortCraftButtons(Entity target_entity, const std::vector<Entity>& buttons, std::vector<Entity>& sorted_buttons)
	{
		
	}

	void update(Entity target_entity)
	{
		
	}

	void render(graphics::Renderer& screen, Entity target_entity)
	{
		//Render Craft Buttons
		std::vector<Entity> buttons_craftable;
		std::vector<Entity> buttons_missing_resources;
		std::vector<Entity> buttons_recipe_locked;

		if (!registry.all_of<Components::HasInventory>(target_entity)) return;

		const auto& inventory = registry.get<Components::HasInventory>(target_entity).inventory;

		//Loop through all craft buttons and split them in 3 arrays for rendering order
		auto view = registry.view<Components::Transform, Components::Button, Components::CraftButton>();
		for (auto [entity, ts, craft_button_component] : view.each())
		{
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
			auto& transform_component = registry.get<Components::Transform>(button);
			const auto& craft_button_component = registry.get<Components::CraftButton>(button);
			
			int x = i % ui_settings.craft_button_columns;
			int y = i / ui_settings.craft_button_columns;

			transform_component.position.x = craft_button_position_x + x * ui_settings.craft_button_width;
			transform_component.position.y = craft_button_position_y + y * ui_settings.craft_button_height;
			transform_component.size.x = ui_settings.craft_button_width;
			transform_component.size.y = ui_settings.craft_button_height;

			size_t item_id = CraftingManager::get().getRecipe(craft_button_component.recipe_id).item_id;
			int item_sprite = ItemManager::get().getProperties(item_id).sprite_index;

			graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("ui"))[0], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, graphics::IGNORE_VIEW_ZOOM);
			graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("items"))[item_sprite], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, graphics::IGNORE_VIEW_ZOOM);

			++i;
		}

		for (const auto& button : buttons_missing_resources)
		{
			auto& transform_component = registry.get<Components::Transform>(button);
			const auto& craft_button_component = registry.get<Components::CraftButton>(button);

			int x = i % ui_settings.craft_button_columns;
			int y = i / ui_settings.craft_button_columns;

			transform_component.position.x = craft_button_position_x + x * ui_settings.craft_button_width;
			transform_component.position.y = craft_button_position_y + y * ui_settings.craft_button_height;
			transform_component.size.x = ui_settings.craft_button_width;
			transform_component.size.y = ui_settings.craft_button_height;

			size_t item_id = CraftingManager::get().getRecipe(craft_button_component.recipe_id).item_id;
			int item_sprite = ItemManager::get().getProperties(item_id).sprite_index;

			graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("ui"))[0], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, graphics::IGNORE_VIEW_ZOOM);
			graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("items"))[item_sprite], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, graphics::IGNORE_VIEW_ZOOM);
			graphics::drawRectangle(screen, transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, graphics::RenderType::FILL, graphics::Color{0,0,0,125}, graphics::IGNORE_VIEW_ZOOM);

			++i;
		}

		for (const auto& button : buttons_recipe_locked)
		{
			auto& transform_component = registry.get<Components::Transform>(button);
			const auto& craft_button_component = registry.get<Components::CraftButton>(button);

			int x = i % ui_settings.craft_button_columns;
			int y = i / ui_settings.craft_button_columns;

			transform_component.position.x = craft_button_position_x + x * ui_settings.craft_button_width;
			transform_component.position.y = craft_button_position_y + y * ui_settings.craft_button_height;
			transform_component.size.x = ui_settings.craft_button_width;
			transform_component.size.y = ui_settings.craft_button_height;

			graphics::drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("ui"))[1], transform_component.position.x, transform_component.position.y, transform_component.size.x, transform_component.size.y, graphics::IGNORE_VIEW_ZOOM);

			++i;
		}
	}

private:
	entt::registry& registry;
	const UISettings& ui_settings;
};
