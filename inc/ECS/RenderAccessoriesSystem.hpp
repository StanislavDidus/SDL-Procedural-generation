#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "Entity.hpp"
#include "ItemManager.hpp"
#include "ResourceManager.hpp"
#include "UI/UISettings.hpp"

class RenderAccessoriesSystem
{
public:
	RenderAccessoriesSystem(entt::registry& registry, const UISettings& ui_settings, std::shared_ptr<ItemDescriptionSystem> item_description_system) 
		: registry{registry}
		, ui_settings{ ui_settings }
		, item_description_system{item_description_system}
	{
	}

	void update(Entity target_entity, graphics::GpuRenderer& screen)
	{
		using AccessoryRect = std::pair<Entity, SDL_FRect>;
		std::vector<AccessoryRect> accessory_rect;

		if (registry.all_of<Components::Equipment>(target_entity))
		{
			const auto& equipment = registry.get<Components::Equipment>(target_entity);

			int i = 0;
			for (const auto& accessory : equipment.accessories)
			{
				const auto& item_properties = ItemManager::get().getProperties(registry, accessory);
				int sprite_index = item_properties.sprite_index;

				int x = i % ui_settings.accessories_columns;
				int y = i / ui_settings.accessories_columns;

				if (y >= ui_settings.accessories_rows) break;

				SDL_FRect rect =
				{
					x * ui_settings.accessories_icon_size_x + ui_settings.accessories_position_x,
					y * ui_settings.accessories_icon_size_y + ui_settings.accessories_position_y,
					ui_settings.accessories_icon_size_x,
					ui_settings.accessories_icon_size_y,
				};

				graphics::drawScaledSprite
				(
					screen,
					ResourceManager::get().getSpriteSheet("items")->getSprite(sprite_index),
					rect.x,
					rect.y,
					rect.w,
					rect.h,
					graphics::IGNORE_VIEW_ZOOM
				);

				accessory_rect.emplace_back(accessory, rect);

				++i;
			}
		}

		for (const auto& [entity, rect] : accessory_rect)
		{
			if (isMouseIntersection(InputManager::getMouseState().position, rect))
			{
				item_description_system->drawItemDescription(screen, rect.x, rect.y - rect.h, entity);
			}
		}
	}

private:
	entt::registry& registry;
	const UISettings& ui_settings;
	std::shared_ptr<ItemDescriptionSystem> item_description_system;
};
