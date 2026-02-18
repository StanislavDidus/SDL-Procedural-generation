#pragma once
#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"

struct CircleWeapon
{
	const Item* item;
	float distance;
	float current_angle;
	bool flip = false;
};

class RenderWeaponCircle
{
public:
	RenderWeaponCircle(Entity target_entity) : target_entity{target_entity}
	{
		const auto& component_manager = ComponentManager::get();
		if (!component_manager.equipment.contains(target_entity)) return;

		const auto& player_equipment_component = component_manager.equipment.at(target_entity);

		PlaceCircleSlots(player_equipment_component);
	}

	void PlaceCircleSlots(const Equipment& equipment)
	{
		// Instantiate empty slots around a player
		float angle_between = 360.0f / equipment.max_weapon;
		for (int i = 0; i < equipment.max_weapon; ++i)
		{
			circle_slots.emplace_back(nullptr, 35.0f, angle_between * i, false);
		}
	}

	void update(float dt)
	{
		const auto& component_manager = ComponentManager::get();
		if (!component_manager.transform.contains(target_entity) || !component_manager.equipment.contains(target_entity)) return;

		const auto& player_transform_component = component_manager.transform.at(target_entity);
		const auto& player_equipment_component = component_manager.equipment.at(target_entity);

		// Add new circle_slots to the "Circle"
		if (component_manager.item_equipped.contains(target_entity))
		{
			const auto& item_equipped_component = component_manager.item_equipped.at(target_entity);
			const auto& item_properties = ItemManager::get().getProperties(item_equipped_component.item->id);
			
			// Store the last weapon capacity of the player
			// Reconstruct circle weapon slots if values are not the same
			if (last_weapon_capacity != player_equipment_component.max_weapon)
			{
				PlaceCircleSlots(player_equipment_component);
			}
			last_weapon_capacity = player_equipment_component.max_weapon;

			// If item is not a pickaxe
			if (!item_properties.pickaxe_data)
			{
				auto free_circle_slot = std::ranges::find_if(circle_slots, [](const CircleWeapon& cw) {return cw.item == nullptr; });
				if (free_circle_slot != circle_slots.end())
				{
					auto& slot = *free_circle_slot;
					slot.item = item_equipped_component.item;
				}
			}
		}
		
		// Unequip item
		if (component_manager.item_unequipped.contains(target_entity))
		{
			/*circle_slots.erase(std::ranges::remove_if(circle_slots,))*/
			const auto& item_unequipped_component = component_manager.item_unequipped.at(target_entity);
			//circle_slots.erase(std::ranges::remove_if(circle_slots, [unequip_item_component](const CircleWeapon& display_weapon) {return display_weapon.item == unequip_item_component.item; }).begin(), circle_slots.end());

			for (auto& slot : circle_slots)
			{
				if (slot.item == item_unequipped_component.item) slot.item = nullptr;
			}
		}
		
		// Rotate all circle slots
		for (auto& weapon : circle_slots)
		{
			weapon.current_angle += 50.0f * dt;
			weapon.current_angle = std::fmodf(weapon.current_angle, 360.0f);
			weapon.flip = weapon.current_angle >= 180.0f;
		}

	}

	void render(graphics::Renderer& screen) const
	{
		const float WeaponSize = 30.0f;
		const auto& component_manager = ComponentManager::get();
		if (component_manager.transform.contains(target_entity))
		{
			const auto& player_transform_position = component_manager.transform.at(target_entity);

			for (const auto& weapon : circle_slots)
			{
				if (!weapon.item) continue;
				glm::vec2 player_centre_position = glm::vec2{ player_transform_position.position + player_transform_position.size * 0.5f };
				float x = player_centre_position.x - WeaponSize * 0.5f;
				float y = player_centre_position.y - weapon.distance;

				float radians = weapon.current_angle * (3.1416f / 180.0f);
				float cos = std::cos(radians);
				float sin = std::sin(radians);

				glm::vec2 relative_position = glm::vec2{x, y} - player_centre_position;
				glm::vec2 rotated_position =
				{
					cos * relative_position.x - sin * relative_position.y + player_centre_position.x,		
					sin * relative_position.x + cos * relative_position.y + player_centre_position.y
				};

				const auto& item_properties = ItemManager::get().getProperties(weapon.item->id);
				int sprite_index = item_properties.sprite_index;

				if (weapon.flip)
					graphics::drawRotatedSprite(screen, (*ResourceManager::get().getSpriteSheet("items"))[sprite_index], rotated_position.x - WeaponSize * 0.5f, rotated_position.y - WeaponSize * 0.5f, WeaponSize, WeaponSize, 0.f, SDL_FLIP_HORIZONTAL);
				else
					graphics::drawRotatedSprite(screen, (*ResourceManager::get().getSpriteSheet("items"))[sprite_index], rotated_position.x - WeaponSize * 0.5f, rotated_position.y - WeaponSize * 0.5f, WeaponSize, WeaponSize, 0.f, SDL_FLIP_NONE);
			}
		}
	}

private:
	std::vector<CircleWeapon> circle_slots;
	Entity target_entity;

	int last_weapon_capacity = 0;
};
