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

constexpr float LABEL_WIDTH = 250.f;
constexpr float LABEL_HEIGHT = 25.f;

const float ID_POSITION_X = 200.f;

constexpr float RESOURCE_ICON_WIDTH = 50.f;
constexpr float RESOURCE_ICON_HEIGHT = 50.f;

constexpr float DESCRIPTION_Y_STEP = 50.f;

constexpr float ITEM_COMPONENTS_OFFSET_X = 10.f;

class ItemDescriptionSystem
{
public:
	ItemDescriptionSystem(ComponentManager& component_manager, const EntityManager& entity_manager, const Font* font, std::shared_ptr<InventoryView> inventory_view)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, font(font)
		, inventory_view(inventory_view)
	{

	}

	template<typename T>
	static T* getItemComponent(const std::vector<std::unique_ptr<ItemComponent>>& components)
	{
		auto it = std::ranges::find_if(components, [](const std::unique_ptr<ItemComponent>& other)
			{
				return	dynamic_cast<T*>(other.get()) != nullptr;
			});

		return it != components.end() ? static_cast<T*>(it->get()) : nullptr;
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

	void render(Renderer& screen, Entity target_entity) const
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

				float draw_x = slot_global_position.x - LABEL_WIDTH * 0.5f;
				float draw_y = slot_global_position.y + slot_size.y;

				const auto& item_components = ItemManager::get().getProperties(item->id).components;

				//Count how many properties an item has
				int number_item_properties = 0;
				for (const auto& component : item_components)
				{
					number_item_properties += component->number_properties;
				}

				const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());

				//Clamp the position to a screen
				draw_x = std::max(0.f, draw_x);
				draw_y = std::max(0.f, draw_y);

				draw_x = std::min(draw_x, window_size.x - LABEL_WIDTH);
				draw_y = std::min(draw_y, window_size.y - (LABEL_HEIGHT + number_item_properties * RESOURCE_ICON_HEIGHT));

				renderDescriptionLabel(screen, draw_x, draw_y, item->id, number_item_properties);

				renderItemComponents(screen, draw_x + ITEM_COMPONENTS_OFFSET_X, draw_y + LABEL_HEIGHT, item_components);
			}
			//Otherwise check if mouse is on any of the crafting recipes
			else
			{
				for (const auto& entity : entity_manager.getEntities())
				{
					if (!component_manager.transform.contains(entity) || !component_manager.button_covered.contains(entity) || !component_manager.craft_button.contains(entity) || !component_manager.has_inventory.contains(target_entity)) continue;
					{
						const auto& transform_component = component_manager.transform.at(entity);
						const auto& craft_button_component = component_manager.craft_button.at(entity);
						const auto& mouse_position = InputManager::getMouseState().position;
						const auto& inventory = component_manager.has_inventory.at(target_entity).inventory;

						if (!craft_button_component.is_available) continue;

						const auto& recipe = CraftingManager::get().getRecipe(craft_button_component.recipe_id);
						size_t item_id = recipe.item_id;
						const auto& item_properties = ItemManager::get().getItem(item_id);
						const auto& item_components = item_properties.components;

						glm::vec2 button_centre = transform_component.position + transform_component.size * 0.5f;
						float x = button_centre.x - LABEL_WIDTH * 0.5f;
						float y = transform_component.position.y + transform_component.size.y;

						int number_item_properties = 0;
						int required_craft_items = recipe.required_items.size();

						//Count how many properties an item has
						for (const auto& component : item_components)
						{
							number_item_properties += component->number_properties;
						}

						const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());

						//Clamp the position to a screen
						x = std::max(0.f, x);
						y = std::max(0.f, y);

						x = std::min(x, window_size.x - LABEL_WIDTH);
						y = std::min(y, window_size.y - (LABEL_HEIGHT + number_item_properties * RESOURCE_ICON_HEIGHT));

						renderDescriptionLabel(screen, x, y, item_id, std::max(number_item_properties, required_craft_items));

						renderItemRecipe(screen, x, y, recipe.required_items, *inventory);

						renderItemComponents(screen, x + RESOURCE_ICON_WIDTH * 2.f + ITEM_COMPONENTS_OFFSET_X, y + LABEL_HEIGHT, item_components);
					}
				}
			}

		}
	}

private:
	void renderDescriptionLabel(Renderer& screen, float x, float y, size_t item_id, int additional_space_height) const
	{
		float additional_height = LABEL_HEIGHT + additional_space_height * RESOURCE_ICON_HEIGHT;

		screen.drawRectangle(x, y, LABEL_WIDTH, additional_height, RenderType::FILL, Color{ 0,125,200,200 }, IGNORE_VIEW_ZOOM);

		const auto& item_properties = ItemManager::get().getProperties(item_id);
		Text item_name_text{ font, screen, item_properties.name };
		Text item_id_text{ font, screen, "ID: " + std::to_string(item_id), SDL_Color{175, 175,175,255} };

		screen.printTextScaled(item_name_text, x, y, 0.6f, 0.6f, IGNORE_VIEW_ZOOM);
		screen.printTextScaled(item_id_text, x + ID_POSITION_X, y, 0.5f, 0.5f, IGNORE_VIEW_ZOOM);
	}

	void renderItemRecipe(Renderer& screen, float x, float y, const std::vector<Item>& recipe, const Inventory& inventory) const
	{
		for (int i = 0; const auto& required_item : recipe)
		{
			const auto& required_item_properties = ItemManager::get().getProperties(required_item.id);
			int sprite_index = required_item_properties.sprite_index;
			int required_item_number = required_item.stack_number;

			float new_y = y + DESCRIPTION_Y_STEP * i + LABEL_HEIGHT;

			int item_number_entity_has = inventory.countItem(required_item.id);
			std::string item_number_string = std::to_string(item_number_entity_has) + "/" + std::to_string(required_item_number);

			SDL_Color text_color = item_number_entity_has >= required_item_number ? SDL_Color{ 0,255,0,255 } : SDL_Color{ 255,0,0,255 };

			drawSpriteWithText(screen, item_number_string, ResourceManager::get().getSpriteSheet("items")[sprite_index], x, new_y, text_color);

			++i;
		}
	}

	void renderItemComponents(Renderer& screen, float x, float y, const std::vector<std::unique_ptr<ItemComponent>>& item_components) const
	{
		auto* heal = getItemComponent<ItemComponents::Heal>(item_components);
		auto* pickaxe = getItemComponent<ItemComponents::Pickaxe>(item_components);
		auto* meleeweapon = getItemComponent<ItemComponents::MeleeWeapon>(item_components);

		SDL_Color text_color = { 255,255,255,255 };
		if (heal)
		{
			//Render heal amount
			renderComponentValue(screen, "heal", heal->value, x, y, text_color);
			y += DESCRIPTION_Y_STEP;
		}
		if (pickaxe)
		{

			//Render Mining Speed
			{
				renderComponentValue(screen, "speed", pickaxe->speed, x, y, text_color);
				y += DESCRIPTION_Y_STEP;
			}

			//Render Mining Radius
			{
				renderComponentValue(screen, "radius", pickaxe->radius, x, y, text_color);
				y += DESCRIPTION_Y_STEP;
			}

			//Render Mining Size
			{
				renderComponentValue(screen, "size", pickaxe->size, x, y, text_color);
				y += DESCRIPTION_Y_STEP;
			}
		}
		if (meleeweapon)
		{
			SDL_Color color = { 255,255,255,255 };
			//Render MeleeWeapon damage
			{
				renderComponentValue(screen, "damage", meleeweapon->damage, x, y, text_color);
				y += DESCRIPTION_Y_STEP;
			}

			//Render MeleeWeapon cooldown
			{
				renderComponentValue(screen, "cooldown", meleeweapon->cooldown, x, y, text_color);
				y += DESCRIPTION_Y_STEP;
			}

			//Render MeleeWeapon radius
			{
				renderComponentValue(screen, "radius", meleeweapon->radius, x, y, text_color);
				y += DESCRIPTION_Y_STEP;
			}
		}
	}

	template <typename T>
	void renderComponentValue(Renderer& screen, const std::string& component_name, const T& component_value, float x, float y, SDL_Color color) const
	{
		std::string text{ };
		text += component_name + ": ";
		text += std::to_string(component_value);

		removeDigitsAfterComma(text, 1);
		Text print_text{ font, screen, text, color };
		screen.printTextScaled(print_text, x, y + 12.5f, 0.5f, 0.5f, IGNORE_VIEW_ZOOM);
	}

	void drawSpriteWithText(Renderer& screen, const std::string& text, const Sprite& sprite, float x, float y, SDL_Color text_color) const
	{
		Text pickaxe_text{ font, screen, text, text_color };
		screen.drawScaledSprite(sprite, x, y, RESOURCE_ICON_WIDTH, RESOURCE_ICON_HEIGHT, IGNORE_VIEW_ZOOM);
		screen.printTextScaled(pickaxe_text, x + RESOURCE_ICON_WIDTH, y + 12.5f, 0.6f, 0.6f, IGNORE_VIEW_ZOOM);
	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;
	const Font* font;	
	std::weak_ptr<InventoryView> inventory_view;

	std::vector<std::unique_ptr<Text>> texts;
};
