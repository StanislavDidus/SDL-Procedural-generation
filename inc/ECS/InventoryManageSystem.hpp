#pragma once
#include "ComponentManager.hpp"

class InventoryManageSystem
{
public:
	InventoryManageSystem(entt::registry& registry) : registry{registry} {}

	void update()
	{
		auto view = registry.view<Components::HasInventory, Components::AddItem>();
		for (auto [entity, inventory_component, add_item_component] : view.each())
		{
			const auto& item = add_item_component.item;
			
			//if (inventory_component.inventory->full()) continue;

			bool result = inventory_component.inventory->addItem(item.id, item.stack_number);

			// If adding an item to the inventory failed
			// We drop it
			if (result == false)
			{
				registry.emplace<Components::DropItem>(entity, item);
			}

			registry.erase<Components::AddItem>(entity);
		}
	}

private:
	entt::registry& registry;
};
