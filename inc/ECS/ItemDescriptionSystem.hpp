#pragma once
#include "Color.hpp"
#include "ComponentManager.hpp"
#include "CraftingManager.hpp"
#include "EntityManager.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ItemManager.hpp"
#include "Font.hpp"
#include "ResourceManager.hpp"
#include "ViewportGuard.hpp"
#include "UI/UISettings.hpp"

class ItemDescriptionSystem
{
public:
	ItemDescriptionSystem(entt::registry& registry, const graphics::Font* font, std::shared_ptr<InventoryView> inventory_view, const UISettings& ui_settings)
		: font(font)
		, inventory_view(inventory_view)
		, ui_settings(ui_settings)
		, registry{registry}
	{

	}

	static void removeDigitsAfterComma(std::string& text, int digits_after_comma)
	{
		int comma_position = 0;
		bool has_found_point = false;
		for (int i = 0; i < text.size(); i++)
		{
			if (text.c_str()[i] == '.')
			{
				has_found_point = true;
				comma_position = i;
				break;
			}
		}

		if (comma_position + digits_after_comma + 1 > text.size()) return;
		if (!has_found_point) return;

		text.erase(text.begin() + comma_position + digits_after_comma + 1, text.end());
	}

	void render(graphics::Renderer& screen, Entity target_entity) const
	{
		if (auto inventory_view_s = inventory_view.lock())
		{
			auto covered_slot = inventory_view_s->getCoveredSlotIndex();

			//If mouse is covering an inventory slot 
			//Then we render item description
			if (covered_slot)
			{
				const auto& item = inventory_view_s->getItem(*covered_slot);
				if (!item) return;

				//Get the middle position of an inventory slot that the cursor is currently on.
				const auto& slot_size = inventory_view_s->getSlotSize();
				auto slot_global_position = inventory_view_s->getSlotGlobalCoords(*covered_slot);
				slot_global_position.x += slot_size.x * 0.5f;

				float draw_x = slot_global_position.x - ui_settings.item_description_label_width * 0.5f;
				float draw_y = slot_global_position.y + slot_size.y;

				const auto& item_properties = ItemManager::get().getProperties(item->id);

				//Count how many properties an item has
				int number_item_properties = 0;
				countProperties(item_properties, number_item_properties);

				const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());

				//Clamp the position to a screen
				draw_x = std::max(0.f, draw_x);
				draw_y = std::max(0.f, draw_y);

				draw_x = std::min(draw_x, window_size.x - ui_settings.item_description_label_width);
				draw_y = std::min(draw_y, window_size.y - (ui_settings.item_description_label_height + number_item_properties * ui_settings.item_description_icon_height));

				renderDescriptionLabel(screen, draw_x, draw_y, item->id, number_item_properties);

				renderItemComponents(screen, draw_x + ui_settings.item_description_components_offset_x, draw_y + ui_settings.item_description_label_height, item_properties);
			}
			//Otherwise check if mouse is on any of the crafting recipes
			else
			{
				auto view = registry.view<Components::Transform, Components::ButtonCovered, Components::CraftButton, Components::HasInventory>();
				for (auto [entity, transform_component, craft_button_component, inventory_component] : view.each())
				{
					const auto& mouse_position = InputManager::getMouseState().position;
					const auto& inventory = inventory_component.inventory;
					if (!craft_button_component.is_available) continue;

					const auto& recipe = CraftingManager::get().getRecipe(craft_button_component.recipe_id);
					size_t item_id = recipe.item_id;
					const auto& item_properties = ItemManager::get().getItem(item_id);
					

					glm::vec2 button_centre = transform_component.position + transform_component.size * 0.5f;
					float x = button_centre.x - ui_settings.item_description_label_width * 0.5f;
					float y = transform_component.position.y + transform_component.size.y;

					
					int required_craft_items = recipe.required_items.size();

					//Count how many properties an item has
					int number_item_properties = 0;
					countProperties(item_properties, number_item_properties);

					const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());

					//Clamp the position to a screen
					x = std::max(0.f, x);
					y = std::max(0.f, y);

					x = std::min(x, window_size.x - ui_settings.item_description_label_width);
					y = std::min(y, window_size.y - (ui_settings.item_description_label_height + number_item_properties * ui_settings.item_description_icon_height));

					renderDescriptionLabel(screen, x, y, item_id, std::max(number_item_properties, required_craft_items));

					renderItemRecipe(screen, x, y, recipe.required_items, *inventory);

					renderItemComponents(screen, x + ui_settings.item_description_icon_width * 2.f + ui_settings.item_description_components_offset_x, y + ui_settings.item_description_label_height, item_properties);
				}
			}

		}
	}

private:
	void countProperties(const ItemProperties& item_properties, int& number_properties) const
	{
		if (item_properties.heal_data)
		{
			number_properties += 1;
		}
		if (item_properties.pickaxe_data)
		{
			number_properties += 3;
		}
		if (item_properties.melee_weapon_data)
		{
			number_properties += 3;
		}
	}

	void renderDescriptionLabel(graphics::Renderer& screen, float x, float y, size_t item_id, int additional_space_height) const
	{
		float additional_height = ui_settings.item_description_label_height + additional_space_height * ui_settings.item_description_icon_height;

		graphics::drawRectangle(screen, x, y, ui_settings.item_description_label_width, additional_height, graphics::RenderType::FILL, graphics::Color{ 0,125,200,200 }, graphics::IGNORE_VIEW_ZOOM);

		const auto& item_properties = ItemManager::get().getProperties(item_id);
		graphics::Text item_name_text{ font, screen, item_properties.name };
		graphics::Text item_id_text{ font, screen, "ID: " + std::to_string(item_id), graphics::Color{175, 175,175,255} };

		graphics::printTextScaled(screen, item_name_text, x, y, ui_settings.item_name_text_scale_x, ui_settings.item_name_text_scale_y, graphics::IGNORE_VIEW_ZOOM);
		graphics::printTextScaled(screen, item_id_text, x + ui_settings.item_description_id_position_x, y, ui_settings.item_id_text_scale_x, ui_settings.item_id_text_scale_y, graphics::IGNORE_VIEW_ZOOM);
	}

	void renderItemRecipe(graphics::Renderer& screen, float x, float y, const std::vector<Item>& recipe, const Inventory& inventory) const
	{
		for (int i = 0; const auto& required_item : recipe)
		{
			const auto& required_item_properties = ItemManager::get().getProperties(required_item.id);
			int sprite_index = required_item_properties.sprite_index;
			int required_item_number = required_item.stack_number;

			float new_y = y + ui_settings.item_description_step_y * i + ui_settings.item_description_label_height;

			int item_number_entity_has = inventory.countItem(required_item.id);
			std::string item_number_string = std::to_string(item_number_entity_has) + "/" + std::to_string(required_item_number);

			graphics::Color text_color = item_number_entity_has >= required_item_number ? graphics::Color{ 0,255,0,255 } : graphics::Color{ 255,0,0,255 };

			drawSpriteWithText(screen, item_number_string, (*ResourceManager::get().getSpriteSheet("items"))[sprite_index], x, new_y, text_color);
			
			++i;
		}
	}

	void renderItemComponents(graphics::Renderer& screen, float x, float y, const ItemProperties& item_properties) const
	{
		graphics::Color text_color = { 255,255,255,255 };

		if (item_properties.heal_data)
		{
			renderComponentValue(screen, "heal", item_properties.heal_data->amount, x, y, text_color);
			y += ui_settings.item_description_step_y;
		}

		if (item_properties.pickaxe_data)
		{

			//Render Mining Speed
			{
				renderComponentValue(screen, "speed", item_properties.pickaxe_data->speed, x, y, text_color);
				y += ui_settings.item_description_step_y;
			}

			//Render Mining Radius
			{
				renderComponentValue(screen, "radius", item_properties.pickaxe_data->radius, x, y, text_color);
				y += ui_settings.item_description_step_y;
			}

			//Render Mining Size
			{
				renderComponentValue(screen, "size", item_properties.pickaxe_data->size, x, y, text_color);
				y += ui_settings.item_description_step_y;
			}
		}

		if (item_properties.melee_weapon_data)
		{
			graphics::Color color = { 255,255,255,255 };
			//Render MeleeWeapon damage
			{
				renderComponentValue(screen, "damage", item_properties.melee_weapon_data->damage, x, y, text_color);
				y += ui_settings.item_description_step_y;
			}

			//Render MeleeWeapon cooldown
			{
				renderComponentValue(screen, "cooldown", item_properties.melee_weapon_data->cooldown, x, y, text_color);
				y += ui_settings.item_description_step_y;
			}

			//Render MeleeWeapon radius
			{
				renderComponentValue(screen, "radius", item_properties.melee_weapon_data->radius, x, y, text_color);
				y += ui_settings.item_description_step_y;
			}
		}
	}

	template <typename T>
	void renderComponentValue(graphics::Renderer& screen, const std::string& component_name, const T& component_value, float x, float y, graphics::Color color) const
	{
		std::string text{ };
		text += component_name + ": ";
		text += std::to_string(component_value);

		removeDigitsAfterComma(text, 1);
		graphics::Text print_text{ font, screen, text, color };
		graphics::printTextScaled(screen, print_text, x, y + 12.5f, ui_settings.crafting_component_text_scale_x, ui_settings.crafting_component_text_scale_y, graphics::IGNORE_VIEW_ZOOM);
	}

	void drawSpriteWithText(graphics::Renderer& screen, const std::string& text, const graphics::Sprite& sprite, float x, float y, graphics::Color text_color) const
	{
		graphics::Text pickaxe_text{ font, screen, text, text_color };
		graphics::drawScaledSprite(screen, sprite, x, y, ui_settings.item_description_icon_width, ui_settings.item_description_icon_height, graphics::IGNORE_VIEW_ZOOM);
		graphics::printTextScaled(screen, pickaxe_text, x + ui_settings.item_description_icon_width, y + 12.5f, ui_settings.item_recipe_text_scale_x, ui_settings.item_recipe_text_scale_y, graphics::IGNORE_VIEW_ZOOM);
	}

	const graphics::Font* font;	
	std::weak_ptr<InventoryView> inventory_view;

	const UISettings& ui_settings;

	std::vector<std::unique_ptr<graphics::Text>> texts;

	entt::registry& registry;
};
