#include "Inventory.hpp"
#include "ECS/Systems.hpp"
#include <algorithm>
#include <commctrl.h>
#include <iostream>

using namespace Components;
using namespace Components::InventoryItems;

Inventory::Inventory(entt::registry& registry, int size) 
	: registry{ registry }
	, size(size)
{
	//Fill items and free slots
	items.resize(size);

	for (int i = size - 1; i >= 0; --i)
	{
		free_slots.push_back(i);
	}
}

void Inventory::useItem(int slot, Entity target_entity, entt::registry& registry)
{
	auto& item = items[slot];

	if (!item) return;
	
	const auto& item_properties = registry.get<ItemProperties>(*item);
	auto& item_info = registry.get<Item>(*item);

	switch (item_properties.action)
	{
	case ItemAction::USE:
	{
		registry.emplace<Components::UseItem>(target_entity, item_properties.id);
		item_info.stack_number--;

		if (item_info.stack_number <= 0)
		{
			items[slot].reset();

			free_slots.push_back(slot);
		}
	}
		break;
	case ItemAction::EQUIP:
		if (!item_info.equipped)
		{
			registry.emplace<Components::EquipItem>(target_entity, *item);
		}
		else if (!item_info.equipped)
		{
			registry.emplace<Components::UnequipItem>(target_entity, *item);
		}
		break;
	case ItemAction::NONE:
		break;
	}
}

bool Inventory::addItem(size_t id, int number)
{
	auto item = ItemManager::get().createItem(registry, id, number);
	return addItem(item);
}

bool Inventory::addItem(Entity item_)
{
	for (auto& item : items)
	{
		if (item)
		{
			//Stack two items if they are the same and can stack
			const auto& item_properties = registry.get<Components::InventoryItems::ItemProperties>(*item);
			const auto& item_properties2 = registry.get<Components::InventoryItems::ItemProperties>(item_);
			if (item_properties.name == item_properties2.name && item_properties.can_stack)
			{
				auto& item_component = registry.get<Components::InventoryItems::Item>(*item);
				const auto& item_component2 = registry.get<Components::InventoryItems::Item>(item_);
				item_component.stack_number += item_component2.stack_number;
				return true;
			}
		}
	}

	//Otherwise put the item in an empty slot
	if (!free_slots.empty())
	{
		if (auto free_slot = findFreeSlot()) items[*free_slot] = item_;
		return true;
	}

	return false;
}

void Inventory::removeItemAtSlot(size_t slot)
{
	//Remove the item and free the slot
	if (slot >= 0 && slot < items.size())
	{
		items[slot].reset();

		free_slots.push_back(slot);
	}
}

void Inventory::splitItemTo(int item_slot, int split_slot)
{
	auto& item = items[item_slot];
	auto& split = items[split_slot];
	
	const auto& item_properties = registry.get<ItemProperties>(*item);
	auto& item_info = registry.get<Item>(*item);

	if (item_info.stack_number > 1)
	{

		//If items are the same move one item 
		if (split)
		{
			const auto& split_item_properties = registry.get<ItemProperties>(*split);
			auto& split_item_info = registry.get<Item>(*item);
			if (split_item_properties == item_properties)
			{
				item_info.stack_number -= 1;
				split_item_info.stack_number += 1;
			}
		}
		//If slot is empty, then copy item
		else if (!split)
		{
			item_info.stack_number -= 1;

			split = ItemManager::get().createItem(registry, item_properties.id, 1);

			std::erase_if(free_slots, [split_slot](int x) { return x == split_slot; });
		}
	}
}

void Inventory::stackItems(int old_item, int new_item)
{
	auto& item = items[old_item];
	auto& item_new = items[new_item];

	if (item)
	{
		const auto& item_properties = registry.get<ItemProperties>(*item);
		if (item_properties.can_stack)
		{
			auto& item_new_info = registry.get<Item>(*item_new);
			const auto& item_info = registry.get<Item>(*item);
			item_new_info.stack_number += item_info.stack_number;
			items[old_item] = std::nullopt;
			free_slots.push_back(old_item);
		}
	}
}

void Inventory::moveItem(int old_slot, int new_slot)
{
	std::erase_if(free_slots, [new_slot](int x) { return x == new_slot; });

	if(!items[new_slot]) free_slots.push_back(old_slot);

	std::swap(items[old_slot], items[new_slot]);
}

bool Inventory::hasItem(Entity item) const
{
	int number = 0;
	for (const auto& i : items)
	{
		if (i)
		{
			//If items have the same id - increase the number by the number of items in stack
			const auto& item_properties = registry.get<ItemProperties>(item);
			const auto& count_item_properties = registry.get<ItemProperties>(*i);
			if (item_properties == count_item_properties)
			{
				const auto& count_item_info = registry.get<Item>(*i);
				number += count_item_info.stack_number;
			}
		}
	}

	//If number of items is bigger or equal than required - return true
	const auto& item_info = registry.get<Item>(item);
	return number >= item_info.stack_number;
}

void Inventory::removeItem(Entity item)
{
	const auto& target_item_info = registry.get<Item>(item);
	int target_number = target_item_info.stack_number;
	int number_destroyed = 0;
	for (size_t i = 0; i < items.size(); i++)
	{
		auto& item_ = items[i];
		if (item_)
		{
			const auto& target_item_properties = registry.get<ItemProperties>(item);
			auto& item_info = registry.get<Item>(*item_);
			if (item == *item_)
			{
				//If destroyed number is bigger than a target then we don't need to delete some resources
				if ( number_destroyed + item_info.stack_number > target_number)
				{
					number_destroyed += item_info.stack_number;

					int number_to_save = number_destroyed - target_number;

					item_info.stack_number = number_to_save;
				}
				else
				{
					number_destroyed += item_info.stack_number;

					removeItemAtSlot(i);
				}

				if (number_destroyed >= target_number)
				{
					return;
				}
			}
		}
	}
}

int Inventory::countItem(size_t item_id) const
{
	int number = 0;
	for (const auto& item : items)
	{
		if (item)
		{
			const auto& item_properties = registry.get<ItemProperties>(*item);
			if (item_properties.id == item_id)
			{
				const auto& item_info = registry.get<Item>(*item);
				number += item_info.stack_number;
			}
		}
	}

	return number;
}

bool Inventory::full() const
{
	return free_slots.empty();
}

const std::vector<std::optional<Entity>>& Inventory::getItems() const
{
	return items;
}

void Inventory::printContent() const
{
	/*int i = 0;
	for (auto& item : items)
	{
		if (item)
		{
			Item& item_ = *item;
			const auto& properties = ItemManager::get().getProperties(item_.id);
			std::cout << "Slot<" << i << "> item: " << properties.name << " " << item_.stack_number << std::endl;
		}
		else
		{
			std::cout << "Slot<" << i << "> is empty" << std::endl;
		}
		++i;
	}*/
}

std::optional<int> Inventory::findFreeSlot()
{
	if (free_slots.empty()) return std::nullopt;

	int min = std::numeric_limits<int>::max();
	for (int free_slot : free_slots)
	{
		min = std::min(free_slot, min);
	}

	std::erase_if(free_slots, [min](int value) {return value == min; });
	return min;
}
