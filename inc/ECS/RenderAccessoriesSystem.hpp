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
	RenderAccessoriesSystem(entt::registry& registry, const UISettings& ui_settings, std::shared_ptr<ItemDescriptionSystem> item_description_system = nullptr) 
		: registry{registry}
		, ui_settings{ ui_settings }
		, item_description_system{item_description_system}
	{
	}

	void update(Entity target_entity, graphics::Renderer& screen)
	{
		if (registry.all_of<Components::Equipment>(target_entity))
		{
			const auto& equipment = registry.get<Components::Equipment>(target_entity);

			int i = 0;
			for (const auto& accessory : equipment.accessories)
			{
				const auto& item = ItemManager::get().getProperties(registry, accessory);
				int sprite_index = item.sprite_index;

				int x = i % ui_settings.accessories_columns;
				int y = i / ui_settings.accessories_columns;

				if (y >= ui_settings.accessories_rows) break;

				graphics::drawScaledSprite
				(
					screen,
					ResourceManager::get().getSpriteSheet("items")->getSprite(sprite_index),
					x * ui_settings.accessories_icon_size_x + ui_settings.accessories_position_x,
					y * ui_settings.accessories_icon_size_y + ui_settings.accessories_position_y,
					ui_settings.accessories_icon_size_x,
					ui_settings.accessories_icon_size_y,
					graphics::IGNORE_VIEW_ZOOM
				);

				++i;
			}
		}
	}

private:
	entt::registry& registry;
	const UISettings& ui_settings;
	std::shared_ptr<ItemDescriptionSystem> item_description_system;
};
