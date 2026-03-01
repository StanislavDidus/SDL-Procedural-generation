#pragma once
#include <ranges>

#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "ResourceManager.hpp"

struct DroppedItemData
{
	glm::vec2 position;
	int sprite_index;
	Entity item;
	bool flipped;
	float wait_time = 0.0f; // How much time does a player need to wait before picking up an item
};

/// <summary>
/// Systems that takes care of already dropped items and updates their timers.
/// Additionally, drops items if any target intends to.
/// </summary>
class DropItemSystem
{
public:
	DropItemSystem(entt::registry& registry) : registry{registry} {}

	void update(float dt)
	{
		// Drop item if target intends to 
		std::vector<DroppedItemData> dropped_item_datas;
		auto view = registry.view<Components::DropItem>();
		std::vector<Entity> to_destroy;
		to_destroy.reserve(view.size());
		for (auto [entity, drop_item_component] : view.each())
		{
			if (registry.all_of<Components::Transform, Components::Renderable>(drop_item_component.target))
			{
				const auto& transform_component = registry.get<Components::Transform>(drop_item_component.target);
				const auto& renderable_component = registry.get<Components::Renderable>(drop_item_component.target);

				const auto& item_properties = ItemManager::get().getProperties(registry, drop_item_component.item);

				dropped_item_datas.emplace_back(transform_component.position, item_properties.sprite_index, drop_item_component.item, renderable_component.flip_mode == SDL_FLIP_HORIZONTAL, 3.0f);
				to_destroy.emplace_back(entity);
			}
		}

		// Another way to drop items (without a source)
		auto view_drop2 = registry.view<Components::DropItem2>();
		to_destroy.reserve(view_drop2.size());
		for (auto [entity, drop_item_component] : view_drop2.each())
		{
			const auto& item_properties = ItemManager::get().getProperties(registry, drop_item_component.item);

			dropped_item_datas.emplace_back(drop_item_component.position, item_properties.sprite_index, drop_item_component.item, false, 0.0f);
			to_destroy.emplace_back(entity);
		}

		for (const auto& entity : to_destroy)
		{
			registry.destroy(entity);
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
			drop.time = dropped_item_data.wait_time;
		}

		auto view2 = registry.view<Components::DroppedItem, Components::Transform>();
		auto view3 = registry.view<Components::Transform, Components::HasInventory>();
		for (auto [entity, dropped_item_component, transform_component] : view2.each())
		{
			// Update items timers
			dropped_item_component.timer += dt;

			if (dropped_item_component.timer >= dropped_item_component.time)
			{
				dropped_item_component.can_be_collected = true;
			}
				
			// Move dropped item to the nearest target with inventory
			if (!dropped_item_component.can_be_collected) continue;

			// Search for any other target that can collect this item(has inventory component)
			for (auto [entity_, player_transform_component, inventory_component] : view3.each())
			{
				if (entity == entity_) continue;

				float distance = glm::distance(transform_component.position, player_transform_component.position);

				if (distance < 100.0f)
				{
					auto item_entity = registry.create();
					registry.emplace<Components::PickUpItem>(item_entity, entity_, entity, dropped_item_component.item);
					break;
				}
			}
		}
	}

private:
	entt::registry& registry;
};
