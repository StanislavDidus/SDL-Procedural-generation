#pragma once
#include <ranges>

#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "ResourceManager.hpp"

constexpr float ITEM_ACTIVATE_TIMER = 3.0f; ///< After this time dropped item will be available to pick up.

struct DroppedItemData
{
	glm::vec2 position;
	int sprite_index;
	Item item;
	bool flipped;
};

/// <summary>
/// Systems that takes care of already dropped items and updates their timers.
/// Additionally, drops items if any entity intends to.
/// </summary>
class DropItemSystem
{
public:
	DropItemSystem(entt::registry& registry) : registry{registry} {}

	void update(float dt)
	{
		std::vector<DroppedItemData> dropped_item_datas;
		auto view = registry.view<Components::Transform, Components::DropItem, Components::Renderable>();
		for (auto [entity, transform_component, drop_item_component, renderable_component] : view.each())
		{
			// Drop item if entity intends to 
			const auto& item_properties = ItemManager::get().getProperties(drop_item_component.item.id);

			dropped_item_datas.emplace_back(transform_component.position, item_properties.sprite_index, drop_item_component.item, renderable_component.flip_mode == SDL_FLIP_HORIZONTAL);
			registry.erase<Components::DropItem>(entity);
		}

		// Create all dropped item entities
		for (const auto& dropped_item_data : dropped_item_datas)
		{
			auto drop_item = registry.create();

			auto& ts = registry.emplace<Components::Transform>(drop_item);
			ts.position = dropped_item_data.position;
			ts.size = glm::vec2{ 35.0f, 35.0f };

			registry.emplace<Components::Renderable>(drop_item, (*ResourceManager::get().getSpriteSheet("items"))[dropped_item_data.sprite_index]);

			float direction = dropped_item_data.flipped ? -1.0f : 1.0f;
			auto& ph = registry.emplace<Components::Physics>(drop_item);
			ph.can_move_horizontal = true;
			ph.velocity = glm::vec2{ 100.0f * direction, -200.0f };
			ph.decelaration = 5.0f;

			auto& drop = registry.emplace<Components::DroppedItem>(drop_item);
			drop.item = dropped_item_data.item;
			drop.can_be_collected = false;
			drop.timer = 0.0f;
		}

		std::vector<Entity> dropped_items_to_delete;
		auto view2 = registry.view<Components::DroppedItem, Components::Transform>();
		auto view3 = registry.view<Components::Transform, Components::HasInventory>();
		for (auto [entity, dropped_item_component, transform_component] : view2.each())
		{
			// Update items timers
			dropped_item_component.timer += dt;

			if (dropped_item_component.timer >= ITEM_ACTIVATE_TIMER)
			{
				dropped_item_component.can_be_collected = true;
			}
				
			// Move dropped item to the nearest entity with inventory
			// TODO: The item should not be picked up if entity doesn't have enough space

			if (!dropped_item_component.can_be_collected) continue;

			// Search for any other entity that can collect this item(has inventory component)
			for (auto [entity_, player_transform_component, inventory_component] : view3.each())
			{
				if (entity == entity_) continue;

				float distance = glm::distance(transform_component.position, player_transform_component.position);

				if (distance < 100.0f && inventory_component.inventory->full() == false)
				{
					/*
					inventory_component.inventory->addItem(dropped_item_component.item.id, dropped_item_component.item.stack_number);
					*/
					registry.emplace<Components::AddItem>(entity_, dropped_item_component.item);
					dropped_items_to_delete.emplace_back(entity);
				}
			}
		}

		for (const auto& entity : dropped_items_to_delete)
		{
			registry.destroy(entity);
		}
	}

private:
	entt::registry& registry;
};
