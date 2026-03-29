#pragma once

#include "Components.hpp"

constexpr float DROP_TIME = 3.0f;
constexpr float UPDATE_TIME = 0.25f;
constexpr float SHOW_TIME = 1.0f;

class DropChestLootSystem
{
public:
	DropChestLootSystem(entt::registry& registry) : registry{registry}
	{
		
	}

	void update(float dt, graphics::GpuRenderer& screen)
	{
		auto view = registry.view<Components::Transform, Components::Chest, Components::DropItemChest>();
		for (auto [entity, transform_component, chest_component, drop_item_chest_component ] : view.each())
		{
			drop_item_chest_component.timer += dt;

			if (drop_item_chest_component.timer - drop_item_chest_component.last_update > UPDATE_TIME && DROP_TIME - drop_item_chest_component.timer > SHOW_TIME)
			{
				size_t max_items = ItemManager::get().maxItems();
				int rand = glm::linearRand(0, static_cast<int>(max_items) - 1);
				drop_item_chest_component.last_sprite = ItemManager::get().getProperties(registry, ItemManager::get().getItem(rand)).sprite_index;
				drop_item_chest_component.last_update = drop_item_chest_component.timer;
			}
			else if (DROP_TIME - drop_item_chest_component.timer <= SHOW_TIME)
			{
				drop_item_chest_component.last_sprite = ItemManager::get().getProperties(registry, drop_item_chest_component.item).sprite_index;
			}

			//Render random items
			if (drop_item_chest_component.timer < DROP_TIME)
			{
				const auto& sprite = ResourceManager::get().getSpriteSheet("items")->getSprite(drop_item_chest_component.last_sprite);
				glm::vec2 position = {transform_component.position.x, transform_component.position.y - 50.f};
				glm::vec2 size = {transform_component.size};
				graphics::drawRectangle(screen, position.x, position.y, size.x, size.y, graphics::RenderType::FILL, graphics::Color::BLUE);
				graphics::drawScaledSprite(screen, sprite, position.x, position.y, size.x, size.y);
			}
			//Drop item at the end
			else if (drop_item_chest_component.timer >= DROP_TIME)
			{
				registry.erase<Components::DropItemChest>(entity);
				
				Entity closest_entity = findPlayerNearby(transform_component.position);

				if (closest_entity != entt::null)
				{
					auto item_add = registry.create();
					registry.emplace<Components::AddItem>(item_add, closest_entity, drop_item_chest_component.item);
				}
			}
		}
	}
	
private:
	// Finds the nearest enemy that can collect the item
	Entity findPlayerNearby(const glm::vec2& position) const
	{
		auto view = registry.view<Components::Transform, Components::HasInventory>();
		std::pair<Entity, float> closest_entity{ entt::null, std::numeric_limits<float>::max() };
		for (auto [entity, transform_component, inventory_component] : view.each())
		{
			float distance = glm::distance(position, transform_component.position);

			if (closest_entity.first == entt::null || closest_entity.second > distance)
			{
				closest_entity.first = entity;
				closest_entity.second = distance;
			}
		}

		return closest_entity.first;
	}
	entt::registry& registry;
};