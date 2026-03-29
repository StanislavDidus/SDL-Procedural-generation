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

		std::vector<CircleWeapon> temp = circle_slots;

		circle_slots.clear();
		float angle_between = 360.0f / equipment.max_weapon;
		for (int i = 0; i < equipment.max_weapon; ++i)
		{
			circle_slots.emplace_back(entt::null, 35.0f, angle_between * i, false);
		}

		// Restore the old weapons
		for (const auto& circle_weapon : temp)
		{
			bool result = putItemInSlot(circle_weapon.entity);

			if (!result)
			{
				auto unequip_item_entity = registry.create();
				registry.emplace<Components::UnequipItem>(unequip_item_entity, circle_weapon.entity, target_entity);
			}
		}
	}

	void update(float dt)
	{
		if (!registry.all_of<Components::Transform>(target_entity) || !registry.all_of<Components::Equipment>(target_entity)) return;

		const auto& player_transform_component = registry.get<Components::Transform>(target_entity);
		const auto& player_equipment_component = registry.get<Components::Equipment>(target_entity);

		// Store the last weapon capacity of the player
		// Reconstruct circle weapon slots if values are not the same
		if (last_weapon_capacity != player_equipment_component.max_weapon)
		{
			PlaceCircleSlots(player_equipment_component);
		}
		last_weapon_capacity = player_equipment_component.max_weapon;

		// Add new circle_slots to the "Circle"
		auto view = registry.view<Components::ItemEquipped>();
		for (auto [entity, item_equipped_component] : view.each())
		{
			auto target = item_equipped_component.target;

			if (target == target_entity)
			{
				// If item is a weapon
				if (registry.all_of<Components::InventoryItems::WeaponComponent>(item_equipped_component.item))
				{
					putItemInSlot(item_equipped_component.item);
				}
			}
		}
		
		// Unequip item
		auto view1 = registry.view<Components::ItemUnequipped>();
		for (auto [entity, item_unequipped_component] : view1.each())
		{
			auto target = item_unequipped_component.target;

			if (target == target_entity)
			{
				removeItemInSlot(item_unequipped_component.item);
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

	void render(graphics::GpuRenderer& screen) const
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
	bool putItemInSlot(Entity item) ///< Return false on failure
	{
		auto free_circle_slot = std::ranges::find_if(circle_slots, [](const CircleWeapon& cw) {return cw.entity == entt::null; });
		if (free_circle_slot != circle_slots.end())
		{
			auto& slot = *free_circle_slot;
			slot.entity = item;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void removeItemInSlot(Entity item)
	{
		for (auto& slot : circle_slots)
		{
			if (slot.entity == item) slot.entity = entt::null;
		}
	}

	entt::registry& registry;
	std::vector<CircleWeapon> circle_slots;
	Entity target_entity;

	int last_weapon_capacity = 0;
};
