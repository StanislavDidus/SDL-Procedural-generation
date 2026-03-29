#pragma once

#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"
#include "UI/UISettings.hpp"
#include "RenderFunctions.hpp"

class RenderEssenceCounter
{
public:
	RenderEssenceCounter(const entt::registry& registry, const UISettings& ui_settings, const graphics::Font* font) : registry{ registry }, ui_settings{ ui_settings }, font { font } {}

	void render(graphics::GpuRenderer& screen, Entity target_entity)
	{
		if (!registry.all_of<Components::EquipmentEssence>(target_entity)) return;

		const auto& equipment_essence_component = registry.get<Components::EquipmentEssence>(target_entity);

		const auto& position = ui_settings.essence_counter_position;
		const auto& size = ui_settings.essence_icon_size;

		const float space = ui_settings.essence_text_space;
		float offset_x = 0.0f;

		// Render Common Essence
		{
			const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Common_Essence");
			graphics::drawScaledSprite(screen, sprite, position.x + offset_x, position.y, size.x, size.y, graphics::IGNORE_VIEW_ZOOM);

			offset_x += size.x + space;

			graphics::Text text{ font, screen, std::to_string(equipment_essence_component.common_essence) };	
			glm::vec2 text_size = text.getTextSize(glm::vec2{ ui_settings.essence_text_scale });

			float new_y = position.y + size.y * 0.5f - text_size.y * 0.5;
			graphics::printTextScaled(screen, text, position.x + offset_x, new_y, ui_settings.essence_text_scale, ui_settings.essence_text_scale, graphics::IGNORE_VIEW_ZOOM);	
			offset_x += text_size.x;
		}

		// Render Snow Essence
		{
			const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Snow_Essence");
			graphics::drawScaledSprite(screen, sprite, position.x + offset_x, position.y, size.x, size.y, graphics::IGNORE_VIEW_ZOOM);

			offset_x += size.x + space;

			graphics::Text text{ font, screen, std::to_string(equipment_essence_component.snow_essence) };	
			glm::vec2 text_size = text.getTextSize(glm::vec2{ ui_settings.essence_text_scale });

			float new_y = position.y + size.y * 0.5f - text_size.y * 0.5;
			graphics::printTextScaled(screen, text, position.x + offset_x, new_y, ui_settings.essence_text_scale, ui_settings.essence_text_scale, graphics::IGNORE_VIEW_ZOOM);	
			offset_x += text_size.x;
		}
		// Render Sand Essence
		{
			const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Sand_Essence");
			graphics::drawScaledSprite(screen, sprite, position.x + offset_x, position.y, size.x, size.y, graphics::IGNORE_VIEW_ZOOM);

			offset_x += size.x + space;

			graphics::Text text{ font, screen, std::to_string(equipment_essence_component.sand_essence) };	
			glm::vec2 text_size = text.getTextSize(glm::vec2{ ui_settings.essence_text_scale });

			float new_y = position.y + size.y * 0.5f - text_size.y * 0.5;
			graphics::printTextScaled(screen, text, position.x + offset_x, new_y, ui_settings.essence_text_scale, ui_settings.essence_text_scale, graphics::IGNORE_VIEW_ZOOM);	
			offset_x += text_size.x;
		}
	}

private:
	const entt::registry& registry;
	const UISettings& ui_settings;
	const graphics::Font* font;
};