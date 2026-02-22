#pragma once
#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"

struct CircleWeapon
{
	Entity entity;
	float distance;
	float current_angle;
	bool flip = false;
};

class RenderWeaponCircle
{
public:
	RenderWeaponCircle(entt::registry& registry, Entity target_entity) : target_entity{target_entity}, registry{registry}
	{
		if (!registry.all_of<Components::Equipment>(target_entity)) return;

		const auto& player_equipment_component = registry.get<Components::Equipment>(target_entity);

		PlaceCircleSlots(player_equipment_component);
	}

	void PlaceCircleSlots(const Components::Equipment& equipment)
	{
		// Instantiate empty slots around a player
		float angle_between = 360.0f / equipment.max_weapon;
		for (int i = 0; i < equipment.max_weapon; ++i)
		{
			circle_slots.emplace_back(entt::null, 35.0f, angle_between * i, false);
		}
	}

	void update(float dt)
	{
		if (!registry.all_of<Components::Transform>(target_entity) || !registry.all_of<Components::Equipment>(target_entity)) return;

		const auto& player_transform_component = registry.get<Components::Transform>(target_entity);
		const auto& player_equipment_component = registry.get<Components::Equipment>(target_entity);

		// Add new circle_slots to the "Circle"
		if (registry.all_of<Components::ItemEquipped>(target_entity))
		{
			const auto& item_equipped_component = registry.get<Components::ItemEquipped>(target_entity);
			const auto& item_properties = ItemManager::get().getProperties(registry, item_equipped_component.item);
			
			// Store the last weapon capacity of the player
			// Reconstruct circle weapon slots if values are not the same
			if (last_weapon_capacity != player_equipment_component.max_weapon)
			{
				PlaceCircleSlots(player_equipment_component);
			}
			last_weapon_capacity = player_equipment_component.max_weapon;

			// If item is not a pickaxe
			if (!registry.all_of<Components::InventoryItems::PickaxeComponent>(item_equipped_component.item))
			{
				auto free_circle_slot = std::ranges::find_if(circle_slots, [](const CircleWeapon& cw) {return cw.entity == entt::null; });
				if (free_circle_slot != circle_slots.end())
				{
					auto& slot = *free_circle_slot;
					slot.entity = item_equipped_component.item;
				}
			}
		}
		
		// Unequip item
		if (registry.all_of<Components::ItemUnequipped>(target_entity))
		{
			/*circle_slots.erase(std::ranges::remove_if(circle_slots,))*/
			const auto& item_unequipped_component = registry.get<Components::ItemUnequipped>(target_entity);
			//circle_slots.erase(std::ranges::remove_if(circle_slots, [unequip_item_component](const CircleWeapon& display_weapon) {return display_weapon.item == unequip_item_component.item; }).begin(), circle_slots.end());

			for (auto& slot : circle_slots)
			{
				if (slot.entity == item_unequipped_component.item) slot.entity = entt::null;
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
		if (registry.all_of<Components::Transform>(target_entity))
		{
			const auto& player_transform_position = registry.get<Components::Transform>(target_entity);

			for (const auto& weapon : circle_slots)
			{
				if (weapon.entity == entt::null) continue;
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

				const auto& item_properties = ItemManager::get().getProperties(registry, weapon.entity);
				int sprite_index = item_properties.sprite_index;

				if (weapon.flip)
					graphics::drawRotatedSprite(screen, (*ResourceManager::get().getSpriteSheet("items"))[sprite_index], rotated_position.x - WeaponSize * 0.5f, rotated_position.y - WeaponSize * 0.5f, WeaponSize, WeaponSize, 0.f, SDL_FLIP_HORIZONTAL);
				else
					graphics::drawRotatedSprite(screen, (*ResourceManager::get().getSpriteSheet("items"))[sprite_index], rotated_position.x - WeaponSize * 0.5f, rotated_position.y - WeaponSize * 0.5f, WeaponSize, WeaponSize, 0.f, SDL_FLIP_NONE);
			}
		}
	}

private:
	entt::registry& registry;
	std::vector<CircleWeapon> circle_slots;
	Entity target_entity;

	int last_weapon_capacity = 0;
};
