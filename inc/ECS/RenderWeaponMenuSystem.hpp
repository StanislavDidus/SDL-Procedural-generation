#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"
#include "ResourceManager.hpp"

class RenderWeaponMenuSystem
{
public:
	RenderWeaponMenuSystem(entt::registry& registry, const UISettings& ui_setting) : registry{ registry }, ui_settings(ui_setting) {}

	void render(graphics::Renderer& screen, Entity target_entity)
	{
		const auto& window_size = screen.getWindowSize();
		if (!registry.all_of<Components::Equipment>(target_entity)) return;

		const auto& ui = ui_settings;
		
		auto& equipment_component = registry.get<Components::Equipment>(target_entity);

		int number_of_weapons = equipment_component.max_weapon; 

		float draw_width = ui.weapon_menu_slot_width * number_of_weapons;
		float draw_height = ui.weapon_menu_slot_height;
		float draw_x = window_size.x * 0.5f - draw_width * 0.5f;
		float draw_y = ui.weapon_menu_position_y;

		graphics::drawRectangle(screen, draw_x, draw_y, draw_width, draw_height, graphics::RenderType::FILL, graphics::Color::TRANSPARENT_BLUE, graphics::IGNORE_VIEW_ZOOM);

		int current_draw_item = 0;

		for (const auto& weapon : equipment_component.weapons)
		{
			const auto& weapon_properties = ItemManager::get().getProperties(weapon->id);
			int sprite_index = weapon_properties.sprite_index;
			const auto& sprite = ResourceManager::get().getSpriteSheet("items");

			graphics::drawScaledSprite(screen, (*sprite)[sprite_index], draw_x + ui.weapon_menu_slot_width * current_draw_item, draw_y, ui.weapon_menu_slot_width, ui.weapon_menu_slot_height, graphics::IGNORE_VIEW_ZOOM);

			current_draw_item++;
		}

		graphics::drawRectangle(screen, draw_x + draw_width, draw_y, ui.weapon_menu_slot_width, draw_height, graphics::RenderType::FILL, graphics::Color::TRANSPARENT_GREEN, graphics::IGNORE_VIEW_ZOOM);
		if (equipment_component.pickaxe)
		{
			const auto& pickaxe = equipment_component.pickaxe;
			int sprite_index = ItemManager::get().getProperties(pickaxe->id).sprite_index;
			const auto& sprite = ResourceManager::get().getSpriteSheet("items");

			graphics::drawScaledSprite(screen, (*sprite)[sprite_index], draw_x + ui.weapon_menu_slot_width * number_of_weapons, draw_y, ui.weapon_menu_slot_width, ui.weapon_menu_slot_height, graphics::IGNORE_VIEW_ZOOM);
		}
	}
private:
	entt::registry& registry;
	const UISettings& ui_settings;
};
