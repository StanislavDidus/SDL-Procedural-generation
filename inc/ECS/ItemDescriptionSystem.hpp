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

constexpr float LABEL_WIDTH = 250.f;
constexpr float LABEL_HEIGHT = 25.f;

constexpr float RESOURCE_ICON_WIDTH = 50.f;
constexpr float RESOURCE_ICON_HEIGHT = 50.f;

class ItemDescriptionSystem
{
public:
	ItemDescriptionSystem(ComponentManager& component_manager, const EntityManager& entity_manager, const Font* font)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, font(font)
	{

	}

	void render(Renderer& screen, Entity target_entity)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.button_covered.contains(entity) && component_manager.craft_button.contains(entity) && component_manager.has_inventory.contains(target_entity))
			{
				const auto& transform_component = component_manager.transform.at(entity);
				const auto& craft_button_component = component_manager.craft_button.at(entity);
				const auto& mouse_position = InputManager::getMouseState().position;
				const auto& inventory = component_manager.has_inventory.at(target_entity).inventory;

				if (!craft_button_component.is_available) continue;

				const auto& recipe = CraftingManager::get().getRecipe(craft_button_component.recipe_id);
				size_t item_id = recipe.item_id;
				const auto& item_properties = ItemManager::get().getItem(item_id);

				glm::vec2 button_centre = transform_component.position + transform_component.size * 0.5f;
				float x = button_centre.x - LABEL_WIDTH * 0.5f;
				float y = transform_component.position.y + transform_component.size.y;

				float label_height = LABEL_HEIGHT + recipe.required_items.size() * RESOURCE_ICON_HEIGHT;

				screen.drawRectangle(x, y, LABEL_WIDTH, label_height, RenderType::FILL, Color{ 0,125,200,200 }, IGNORE_VIEW_ZOOM);

				//Display some information about the item

				//Render text
				Text item_name_text{ font, screen, item_properties.name };
				Text item_id_text{ font, screen, "ID: " + std::to_string(item_id), SDL_Color{175, 175,175,255} };

				screen.printTextScaled(item_name_text, x, y, 0.6f, 0.6f, IGNORE_VIEW_ZOOM);
				screen.printTextScaled(item_id_text, x + 200.f, y, 0.5f, 0.5f, IGNORE_VIEW_ZOOM);

				constexpr float step_y = 50.f;

				//Render recipe requirements in the description
				for (int i = 0; const auto& required_item : recipe.required_items)
				{
					const auto& required_item_properties = ItemManager::get().getProperties(required_item.id);
					int sprite_index = required_item_properties.sprite_index;
					int required_item_number = required_item.stack_number;

					float new_y = y + step_y * i + 25.f;
					screen.drawScaledSprite(ResourceManager::get().getSpriteSheet("items")[sprite_index], x, new_y, RESOURCE_ICON_WIDTH, RESOURCE_ICON_HEIGHT, IGNORE_VIEW_ZOOM);

					int item_number_entity_has = inventory->countItem(required_item.id);
					std::string item_number_string = std::to_string(item_number_entity_has) + "/" + std::to_string(required_item_number);

					SDL_Color text_color = item_number_entity_has >= required_item_number ? SDL_Color{ 0,255,0,255 } : SDL_Color{ 255,0,0,255 };
					Text required_item_number_text{ font, screen, item_number_string, text_color };

					screen.printTextScaled(required_item_number_text, x + 50.f, new_y + 12.5f, 0.6f, 0.6f, IGNORE_VIEW_ZOOM);

					++i;
				}

				const auto& item_components = item_properties.components;
				auto it = std::ranges::find_if(item_components, [](const std::unique_ptr<ItemComponent>& other)
				{
					return	dynamic_cast<ItemComponents::Pickaxe*>(other.get()) != nullptr;
				});

				if (it != item_components.end())
				{
					auto* pickaxe = static_cast<ItemComponents::Pickaxe*>(it->get());
				}
			}
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
	const Font* font;

	std::vector<std::unique_ptr<Text>> texts;
};
