#pragma once
#include "ComponentManager.hpp"

class InventoryManageSystem
{
public:
	InventoryManageSystem() = default;

	void update()
	{
		auto& component_manager = ComponentManager::get();

		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (component_manager.has_inventory.contains(entity) && component_manager.add_item.contains(entity))
			{
				auto& inventory_component = component_manager.has_inventory.at(entity);
				const auto& add_item_component = component_manager.add_item.at(entity);

				const auto& item = add_item_component.item;
				
				//if (inventory_component.inventory->full()) continue;

				bool result = inventory_component.inventory->addItem(item.id, item.stack_number);

				// If adding an item to the inventory failed
				// We drop it
				if (result == false)
				{
					component_manager.drop_item[entity] = DropItem{ item };
				}

				component_manager.add_item.erase(entity);
			}
		}
	}
};
