#pragma once
#include <entt/entity/registry.hpp>
#include "Components.hpp"

class RenderHealthBarSystem
{
public:
	RenderHealthBarSystem(entt::registry& registry) : registry{registry} {}

	void render(graphics::GpuRenderer& screen)
	{
		auto view = registry.view<Components::HealthBar, Components::Health>();
		for (auto [entity, health_bar_component, health_component] : view.each())
		{
			float current_health = health_component.current_health;
			float max_health = health_component.max_health;
			float percentage = current_health / max_health;
			//Ensure that percentage cannot be less than a 0
			percentage = std::max(0.0f, percentage);

			// Position of a sprite
			glm::vec2 position = health_bar_component.position;
			glm::vec2 size = health_bar_component.size;
			
			// Position of fillbar
			// Apply some offset
			glm::vec2 bar_position = glm::vec2{position.x + 55.0f, position.y + 15.0f};
			glm::vec2 bar_size = glm::vec2{size.x - 75.0f, size.y - 30.0f};
			glm::vec2 new_size = glm::vec2{ bar_size.x * percentage, bar_size.y };

			graphics::drawRectangle(screen, bar_position.x, bar_position.y, new_size.x, new_size.y, graphics::RenderType::FILL, health_bar_component.color, graphics::IGNORE_VIEW_ZOOM);
			
			// Get HealthBar sprite
			const auto& sprite = *ResourceManager::get().getSpriteSheet("ui");
			graphics::drawScaledSprite(screen, sprite[3], position.x, position.y, size.x, size.y, graphics::IGNORE_VIEW_ZOOM);
		}
	}

private:
	entt::registry& registry;
};
