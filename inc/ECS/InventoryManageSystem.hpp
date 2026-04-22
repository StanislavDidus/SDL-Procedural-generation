#pragma once
#include "ComponentManager.hpp"

class InventoryManageSystem
{
public:
	InventoryManageSystem(entt::registry& registry) : registry{registry} {}

	void update()
	{
		auto view = registry.view<Components::AddItem>();
		std::vector<Entity> to_destroy;
		to_destroy.reserve(view.size());
		for (auto [entity, add_item_component] : view.each())
		{
			const auto& target = add_item_component.target;
			const auto& item = add_item_component.item;
			
			//if (inventory_component.inventory->full()) continue;
			if (registry.all_of<Components::HasInventory>(target))
			{
				bool drop_item = true;
				if (registry.all_of<Components::InventoryItems::Accessory>(item))
				{
					// Check if player has enough space for new accessory
					const auto& equipment = registry.get<Components::Equipment>(target);
					if (equipment.accessories.size() < equipment.max_accessories)
					{
						auto equip_item_entity = registry.create();
						registry.emplace<Components::EquipItem>(equip_item_entity, item, target);
					}
					else
					{
						drop_item = false;
					}
					to_destroy.emplace_back(entity);
				}
				else
				{
					auto& inventory_component = registry.get<Components::HasInventory>(target);
					if (!inventory_component.inventory->addItem(item)) drop_item = false;
					to_destroy.emplace_back(entity);
				}
				
				// If adding an item to the inventory failed
				// We drop it
				if (drop_item == false)
				{
					auto drop_entity = registry.create();
					registry.emplace<Components::DropItem>(drop_entity, target, item);
				}
			}
		}

		auto view2 = registry.view<Components::PickUpItem>();
		for (auto [entity, pickup_component] : view2.each())
		{
			const auto& target = pickup_component.target; // Player 
			const auto& source = pickup_component.source; // Actual item on the floor
			const auto& item = pickup_component.item; // Item's data
			
			//if (inventory_component.inventory->full()) continue;
			if (registry.all_of<Components::HasInventory>(target))
			{
				auto& inventory_component = registry.get<Components::HasInventory>(target);
				bool result = inventory_component.inventory->addItem(item);
				to_destroy.emplace_back(entity);

				if (result)
				{
					registry.destroy(source);
					
					// Play pick up sound
					ResourceManager::get().getSound("Pick up")->play();
				}
				else if (!result)
				{
					registry.erase<Components::PickUpItem>(entity);
				}
			}
		}

		for (const auto& entity : to_destroy)
		{
			registry.destroy(entity);
		}


		//Print how many Items exist in the game at the moment
		//auto item_view = registry.view<Components::InventoryItems::Item>();
		//std::cout << item_view.size() << std::endl;
	}

private:
	entt::registry& registry;
};
