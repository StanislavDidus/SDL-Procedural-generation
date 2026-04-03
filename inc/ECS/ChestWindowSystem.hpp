#pragma once

#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"

class ChestWindowSystem
{
public:
	ChestWindowSystem(entt::registry& registry, std::shared_ptr<graphics::Font> font) : registry{ registry }, font{ font } {}

	void update(Entity target_entity, graphics::GpuRenderer& screen)
	{
		std::vector<Entity> to_destroy;

		// Create a new window if there is no already existing windows
		createWindowIfAsked(screen, to_destroy);

		//If player is too far from the chest - close the window
		destroyWindowsAtDistance(to_destroy);

		for (const auto& d : to_destroy)
		{
			deleteParentEntity(d);
		}

		// Remove all OpenChestComponents
		for (auto [entity, temp] : registry.view<Components::UI::OpenChestWindow>().each())
		{
			registry.erase<Components::UI::OpenChestWindow>(entity);
		}
	}

	void render(graphics::GpuRenderer& screen)
	{
		const auto& window_size = screen.getWindowSize();
		auto view = registry.view<Components::Transform, Components::UI::ChestWindow, Components::UI::ParentWindow>();
		for (auto [entity, transform_component, chest_window_component, parent_window_component] : view.each())
		{
			glm::vec2 window_mid_position = transform_component.position + transform_component.size * 0.5f;
			glm::vec2 unlock_button_size = glm::vec2{200.0f, 100.0f};
			glm::vec2 unlock_button_position = glm::vec2{window_mid_position.x - unlock_button_size.x * 0.5f, transform_component.position.y + transform_component.size.y - unlock_button_size.y + 50.0f};

			graphics::drawScaledSprite
			(
				screen,
				ResourceManager::get().getSpriteSheet("ui")->getSprite("ChestUI"),
				transform_component.position.x,
				transform_component.position.y,
				transform_component.size.x,
				transform_component.size.y,
				graphics::IGNORE_VIEW_ZOOM
			);

			graphics::drawRectangle
			(screen, unlock_button_position.x, unlock_button_position.y, unlock_button_size.x, unlock_button_size.y, graphics::RenderType::FILL, graphics::Color::RED, graphics::IGNORE_VIEW_ZOOM);

			std::string str = "Unlock";
			graphics::Text unlock_text{ screen, font, str };

			graphics::printText(screen, unlock_text, unlock_button_position.x, unlock_button_position.y, unlock_button_size.x, unlock_button_size.y, graphics::IGNORE_VIEW_ZOOM);

			//Draw essences icons
			float step_x = 10.0f;
			float offset_y = 130.0f;

			float new_x = transform_component.position.x + 10.0f;

			new_x += step_x;

			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("ui")->getSprite("Common_Essence"),
				new_x,
				transform_component.position.y + offset_y,
				icon_size.x,
				icon_size.y,
				graphics::IGNORE_VIEW_ZOOM);

			new_x += icon_size.x + step_x;

			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("ui")->getSprite("Snow_Essence"),
				new_x,
				transform_component.position.y + offset_y,
				icon_size.x,
				icon_size.y,
				graphics::IGNORE_VIEW_ZOOM);

			new_x += icon_size.x + step_x;

			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("ui")->getSprite("Sand_Essence"),
				new_x,
				transform_component.position.y + offset_y,
				icon_size.x,
				icon_size.y,
				graphics::IGNORE_VIEW_ZOOM);
		}
	}
private:
	void createWindowIfAsked(const graphics::GpuRenderer& screen, std::vector<Entity>& to_destroy)
	{
		auto window_view = registry.view<Components::UI::ChestWindow>();

		auto view = registry.view<Components::UI::OpenChestWindow>();
		for (auto [entity, chest_window_component] : view.each())
		{
			if (window_view.empty())
			{
				openChestWindow(chest_window_component, screen);

				//Destroy the entity afterward
				to_destroy.emplace_back(entity);
				break;
			}
			
			to_destroy.emplace_back(entity);
		}
	}

	void destroyWindowsAtDistance(std::vector<Entity>& to_destroy) const
	{
		auto view = registry.view<Components::UI::ChestWindow>();
		for (auto [entity, chest_window_component] : view.each())
		{
			auto& player = chest_window_component.target;
			auto& chest = chest_window_component.chest;
			
			if (registry.all_of<Components::Transform, Components::EquipmentEssence>(player) && registry.all_of<Components::Transform>(chest))
			{
				const auto& player_transform_component = registry.get<Components::Transform>(player);
				auto& player_equipment = registry.get<Components::EquipmentEssence>(player);
				const auto& chest_transform_component = registry.get<Components::Transform>(chest);

				//TODO: Use mid positions instead
				float distance = glm::distance(player_transform_component.position, chest_transform_component.position);

				if (distance > 100.0f)
				{
					to_destroy.emplace_back(entity);

					// I also need to return all spent essences to the player
					
					giveEssence(player_equipment, chest_window_component.essence_type, chest_window_component.essence_number, 1);
				}
			}
		}
	}

	void deleteParentEntity(Entity entity) const
	{
		if (registry.all_of<Components::UI::ParentWindow>(entity))
		{
			const auto& parent = registry.get<Components::UI::ParentWindow>(entity);
			for (const auto& button : parent.children) registry.destroy(button);
		}
		registry.destroy(entity);
	}

	void openChestWindow(const Components::UI::OpenChestWindow& chest_window_component, const graphics::GpuRenderer& screen)
	{
		auto& player = chest_window_component.target;
		auto& chest = chest_window_component.chest;


		const auto& window_size = screen.getWindowSize();

		// Create window
		auto chest_window = registry.create();

		auto& ts = registry.emplace<Components::Transform>(chest_window);

		ts.position = {
		window_size.x * 0.5f - menu_size.x * 0.5f,
		window_size.y * 0.5f - menu_size.y * 0.5f
		};
		ts.size = menu_size;

		registry.emplace<Components::UI::ChestWindow>(chest_window, player, chest);
		auto& parent = registry.emplace<Components::UI::ParentWindow>(chest_window);


		// Add all 3 buttons

		float step_x = 10.0f;
		float offset_y = 130.0f;

		float new_x = ts.position.x + 10.0f;

		new_x += step_x;

		for (int i = 0; i < 3; ++i)
		{
			auto button = registry.create();
			parent.children.push_back(button);
			registry.emplace<Components::Button>(button);
			auto& button_ts = registry.emplace<Components::Transform>(button);
			button_ts.position.x = new_x;
			button_ts.position.y = ts.position.y + offset_y;
			button_ts.size = icon_size;

			new_x += icon_size.x + step_x;

			switch (i)
			{
			case 0:
				registry.emplace<Components::ButtonFunction>(button, [this, player, chest_window] { addEssence(EssenceType::COMMON, player, chest_window); });
				break;
			case 1:
				registry.emplace<Components::ButtonFunction>(button, [this, player, chest_window] { addEssence(EssenceType::SNOW, player, chest_window); });
				break;
			case 2:
				registry.emplace<Components::ButtonFunction>(button, [this, player, chest_window] { addEssence(EssenceType::SAND, player, chest_window); });
				break;
			}
		}

		// Add UNLOCK Button

		{
			auto button = registry.create();
			parent.children.push_back(button);

			glm::vec2 window_mid_position = ts.position + ts.size * 0.5f;
			glm::vec2 unlock_button_size = glm::vec2{ 200.0f, 100.0f };
			glm::vec2 unlock_button_position = glm::vec2{ window_mid_position.x - unlock_button_size.x * 0.5f, ts.position.y + ts.size.y - unlock_button_size.y + 50.0f };

			auto& button_ts = registry.emplace<Components::Transform>(button);
			button_ts.position = unlock_button_position;
			button_ts.size = unlock_button_size;

			registry.emplace<Components::Button>(button);

			registry.emplace<Components::ButtonFunction>(button, [this, chest, chest_window]
			{
				if (registry.all_of<Components::UI::ChestWindow>(chest_window))
				{
					const auto& chest_window_component = registry.get<Components::UI::ChestWindow>(chest_window);

					registry.emplace_or_replace<Components::UI::OpenChest>(chest, chest_window, chest_window_component.essence_type, chest_window_component.essence_number);

					deleteParentEntity(chest_window);

				}
			});
		}
	}
	
	void addEssence(const EssenceType button_type, Entity player, Entity chest_window_entity) const
	{
		if (registry.all_of<Components::UI::ChestWindow>(chest_window_entity) && registry.all_of<Components::EquipmentEssence>(player))
		{
			auto& chest_window = registry.get<Components::UI::ChestWindow>(chest_window_entity);
			auto& equipment_component = registry.get<Components::EquipmentEssence>(player);

			if (chest_window.essence_number == 0)
			{
				if (button_type == EssenceType::COMMON && equipment_component.common_essence > 0)
				{
					chest_window.essence_type = button_type;
					chest_window.essence_number++;
					giveEssence(equipment_component, button_type, 1, - 1);
				}
				else if (button_type == EssenceType::SNOW && equipment_component.snow_essence > 0)
				{
					chest_window.essence_type = button_type;
					chest_window.essence_number++;
					giveEssence(equipment_component, button_type, 1, -1);
				}
				else if (button_type == EssenceType::SAND && equipment_component.sand_essence > 0)
				{
					chest_window.essence_type = button_type;
					chest_window.essence_number++;
					giveEssence(equipment_component, button_type, 1, -1);
				}
			}
			else if (chest_window.essence_number > 0)
			{
				if (button_type == EssenceType::COMMON && chest_window.essence_type == EssenceType::COMMON)
				{
					if (equipment_component.common_essence > 0)
					{
						chest_window.essence_number++;
						giveEssence(equipment_component, button_type, 1, -1);
					}
				}
				if (button_type == EssenceType::SNOW && chest_window.essence_type == EssenceType::SNOW)
				{
					if (equipment_component.snow_essence > 0)
					{
						chest_window.essence_number++;
						giveEssence(equipment_component, button_type, 1, -1);
					}
				}
				if (button_type == EssenceType::SAND && chest_window.essence_type == EssenceType::SAND)
				{
					if (equipment_component.sand_essence > 0)
					{
						chest_window.essence_number++;
						giveEssence(equipment_component, button_type, 1, -1);
					}
				}
			}
		}
	}

	void giveEssence(Components::EquipmentEssence& equipment, EssenceType type, int number, int sign) const
	{
		switch (type)
		{
		case EssenceType::COMMON:
			equipment.common_essence += number * sign;
			break;
		case EssenceType::SNOW:
			equipment.snow_essence += number * sign;
			break;
		case EssenceType::SAND:
			equipment.sand_essence += number * sign;
			break;
		}
	}

	entt::registry& registry;
	std::shared_ptr<graphics::Font> font;

	glm::vec2 icon_size = {80.0f,80.0f};
	glm::vec2 menu_size = {300.0f, 300.0f};
};
