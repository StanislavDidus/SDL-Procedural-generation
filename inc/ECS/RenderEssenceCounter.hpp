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
	RenderEssenceCounter(const entt::registry& registry, const UISettings& ui_settings, graphics::GpuRenderer& screen, std::shared_ptr<graphics::Font> font) 
		: registry{ registry }
		, ui_settings{ ui_settings }
		, font { font }
		, common_essence_text(screen, font, " ", graphics::Color::BLACK)
		, snow_essence_text(screen, font, " ", graphics::Color::BLACK)
		, sand_essence_text(screen, font, " ", graphics::Color::BLACK)
	{
		
	}

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

			std::string string = std::to_string(equipment_essence_component.common_essence);
			if (common_essence_text.getText() != string)
			{
				common_essence_text.setText(string);
				common_essence_text.updateText(screen);
			}
			glm::vec2 text_size = common_essence_text.getTextSize(glm::vec2{ ui_settings.essence_text_scale });

			float new_y = position.y + size.y * 0.5f - text_size.y * 0.5;
			graphics::printTextScaled(screen, common_essence_text, position.x + offset_x, new_y, ui_settings.essence_text_scale, ui_settings.essence_text_scale, graphics::IGNORE_VIEW_ZOOM);	
			offset_x += text_size.x;
		}

		// Render Snow Essence
		{
			const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Snow_Essence");
			graphics::drawScaledSprite(screen, sprite, position.x + offset_x, position.y, size.x, size.y, graphics::IGNORE_VIEW_ZOOM);

			offset_x += size.x + space;

			std::string string = std::to_string(equipment_essence_component.snow_essence);
			if (snow_essence_text.getText() != string)
			{
				snow_essence_text.setText(string);
				snow_essence_text.updateText(screen);
			}
			glm::vec2 text_size = snow_essence_text.getTextSize(glm::vec2{ ui_settings.essence_text_scale });

			float new_y = position.y + size.y * 0.5f - text_size.y * 0.5;
			graphics::printTextScaled(screen,snow_essence_text, position.x + offset_x, new_y, ui_settings.essence_text_scale, ui_settings.essence_text_scale, graphics::IGNORE_VIEW_ZOOM);	
			offset_x += text_size.x;
		}
		// Render Sand Essence
		{
			const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Sand_Essence");
			graphics::drawScaledSprite(screen, sprite, position.x + offset_x, position.y, size.x, size.y, graphics::IGNORE_VIEW_ZOOM);

			offset_x += size.x + space;

			std::string string = std::to_string(equipment_essence_component.sand_essence);
			if (sand_essence_text.getText() != string)
			{
				sand_essence_text.setText(string);
				sand_essence_text.updateText(screen);
			}

			glm::vec2 text_size = sand_essence_text.getTextSize(glm::vec2{ ui_settings.essence_text_scale });

			float new_y = position.y + size.y * 0.5f - text_size.y * 0.5;
			graphics::printTextScaled(screen, sand_essence_text, position.x + offset_x, new_y, ui_settings.essence_text_scale, ui_settings.essence_text_scale, graphics::IGNORE_VIEW_ZOOM);	
			offset_x += text_size.x;
		}
	}

private:
	const entt::registry& registry;
	const UISettings& ui_settings;
	std::shared_ptr<graphics::Font> font;

	graphics::Text common_essence_text;
	graphics::Text snow_essence_text;
	graphics::Text sand_essence_text;
};