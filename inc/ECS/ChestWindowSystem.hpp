#pragma once

#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"

class ChestWindowSystem
{
public:
	ChestWindowSystem(entt::registry& registry, const graphics::Font* font) : registry{ registry }, font{ font } {}

	void update(Entity target_entity, graphics::Renderer& screen)
	{
		auto window_view = registry.view<Components::UI::ChestWindow>();

		std::vector<Entity> to_destroy;

		// Create a new window if there is no already existing windows
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

		//If player is too far from the chest - close the window
		auto view2 = registry.view<Components::UI::ChestWindow>();
		for (auto [entity, chest_window_component] : view2.each())
		{
			auto& player = chest_window_component.target;
			auto& chest = chest_window_component.chest;
			
			if (registry.all_of<Components::Transform>(player) && registry.all_of<Components::Transform>(chest))
			{
				const auto& player_transform_component = registry.get<Components::Transform>(player);
				const auto& chest_transform_component = registry.get<Components::Transform>(chest);

				//TODO: Use mid positions instead
				float distance = glm::distance(player_transform_component.position, chest_transform_component.position);

				if (distance > 100.0f)
				{
					to_destroy.emplace_back(entity);
				}
			}
		}
		
		for (const auto& d : to_destroy)
		{
			registry.destroy(d);
		}

		// Remove all OpenChestComponents
		for (auto [entity, temp] : registry.view<Components::UI::OpenChestWindow>().each())
		{
			registry.erase<Components::UI::OpenChestWindow>(entity);
		}
	}

	void render(graphics::Renderer& screen)
	{
		
		auto view = registry.view<Components::Transform, Components::UI::ChestWindow>();
		for (auto [entity, transform_component, chest_window_component] : view.each())
		{
			glm::vec2 window_mid_position = transform_component.position + transform_component.size * 0.5f;
			glm::vec2 unlock_button_size = glm::vec2{200.0f, 100.0f};
			glm::vec2 unlock_button_position = glm::vec2{window_mid_position.x - unlock_button_size.x * 0.5f, transform_component.position.y + transform_component.size.y - unlock_button_size.y + 50.0f};

			graphics::drawRectangle
			(screen, unlock_button_position.x, unlock_button_position.y, unlock_button_size.x, unlock_button_size.y, graphics::RenderType::FILL, graphics::Color::RED, graphics::IGNORE_VIEW_ZOOM);

			std::string str = "Unlock";
			graphics::Text unlock_text{ font, screen, str };

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
	void openChestWindow(const Components::UI::OpenChestWindow& chest_window_component, const graphics::Renderer& screen)
	{
		auto& player = chest_window_component.target;
		auto& chest = chest_window_component.chest;


		const auto& window_size = screen.getWindowSize();

		// Create window
		auto chest_window = registry.create();
		auto& ts = registry.emplace<Components::Transform>(chest_window);
		ts.position = glm::vec2
		{ 
			window_size.x * 0.5f - menu_size.x * 0.5f,
			window_size.y * 0.5f - menu_size.y * 0.5f 
		};
		ts.size = menu_size;

		auto& renderable = registry.emplace<Components::Renderable>(chest_window);
		renderable.sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("ChestUI");
		renderable.ignore_view_zoom = true;

		registry.emplace<Components::UI::ChestWindow>(chest_window, player, chest);
	}

	entt::registry& registry;
	const graphics::Font* font;

	glm::vec2 icon_size = {80.0f,80.0f};
	glm::vec2 menu_size = {300.0f, 300.0f};
};
