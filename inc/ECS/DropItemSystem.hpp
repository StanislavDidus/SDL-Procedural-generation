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
	DropItemSystem() = default;

	void update(float dt)
	{
		std::vector<DroppedItemData> dropped_item_datas;
		auto& component_manager = ComponentManager::get();
		for (const auto& entity : EntityManager::get().getEntities())
		{
			// Drop item if entity intends to 
			if (component_manager.transform.contains(entity) && component_manager.drop_item.contains(entity) && component_manager.renderable.contains(entity))
			{
				const auto& transform_component = component_manager.transform.at(entity);
				auto& drop_item_component = component_manager.drop_item.at(entity);
				const auto& renderable_component = component_manager.renderable.at(entity);
				const auto& item_properties = ItemManager::get().getProperties(drop_item_component.item.id);

				dropped_item_datas.emplace_back(transform_component.position, item_properties.sprite_index, drop_item_component.item, renderable_component.flip_mode == SDL_FLIP_HORIZONTAL);
				component_manager.drop_item.erase(entity);
			}

		}

		// Create all dropped item entities
		for (const auto& dropped_item_data : dropped_item_datas)
		{
			auto drop_item = EntityManager::get().createEntity();
			if (!drop_item) continue;

			component_manager.transform[*drop_item] = Transform
			{
				dropped_item_data.position,
				{35.0f, 35.0f}
			};

			component_manager.renderable[*drop_item] = Renderable
			{
				(*ResourceManager::get().getSpriteSheet("items"))[dropped_item_data.sprite_index]
			};

			float direction = dropped_item_data.flipped ? -1.0f : 1.0f;
			component_manager.physics[*drop_item] = Physics
			{
				true,
				{100.f * direction, -200.0f},
				{},
				{},
				5.0f
			};

			component_manager.dropped_item[*drop_item] = DroppedItem
			{
				dropped_item_data.item,
				false,
				0.0f
			};
		}

		std::vector<Entity> dropped_items_to_delete;
		for (const auto& entity : EntityManager::get().getEntities())
		{
			// Update items timers
			if (component_manager.dropped_item.contains(entity))
			{
				auto& dropped_item_component = component_manager.dropped_item.at(entity);
				dropped_item_component.timer += dt;

				if (dropped_item_component.timer >= ITEM_ACTIVATE_TIMER)
				{
					dropped_item_component.can_be_collected = true;
				}
			}
				
			// Move dropped item to the nearest entity with inventory
			// TODO: The item should not be picked up if entity doesn't have enough space
			
			if (component_manager.transform.contains(entity) && component_manager.dropped_item.contains(entity))
			{
				auto& transform_component = component_manager.transform.at(entity);
				const auto& dropped_item_component = component_manager.dropped_item.at(entity);	

				if (!dropped_item_component.can_be_collected) continue;

				// Search for any other entity that can collect this item(has inventory component)
				for (const auto& entity_ : EntityManager::get().getEntities())
				{
					if (entity == entity_) continue;

					if (component_manager.transform.contains(entity_) && component_manager.has_inventory.contains(entity_))
					{
						const auto& player_transform_position = component_manager.transform.at(entity_);
						auto& inventory_component = component_manager.has_inventory.at(entity_);

						float distance = glm::distance(transform_component.position, player_transform_position.position);

						if (distance < 100.0f && inventory_component.inventory->full() == false)
						{
							/*
							inventory_component.inventory->addItem(dropped_item_component.item.id, dropped_item_component.item.stack_number);
							*/
							component_manager.add_item[entity_] = AddItem{dropped_item_component.item};
							dropped_items_to_delete.emplace_back(entity);
						}
					}
				}
			}
		}

		for (const auto& entity : dropped_items_to_delete)
		{
			removeEntity(entity);
		}
	}
};
