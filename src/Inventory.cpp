#include "Inventory.hpp"
#include "ECS/Systems.hpp"
#include <iostream>

Inventory::Inventory(std::shared_ptr<ItemUsageSystem> item_usage_system, std::shared_ptr<ItemManager> item_manager, int size) 
	: item_usage_system(item_usage_system), item_manager(item_manager), size(size)
{
	//Fill items and free slots
	items.resize(size);

	for (int i = size - 1; i >= 0; --i)
	{
		free_slots.push_back(i);
	}
}

void Inventory::useItem(int slot)
{
	auto& item = items[slot];

	if(item && item_usage_system && item_manager)
	{ 
		bool was_used = item_usage_system->useItem(item_manager->getProperties((item->id)));

		//If useItem returned true than the item was used
		if (was_used)
		{
			item->stack_number--;

			if (item->stack_number <= 0)
			{
				item = std::nullopt;
				free_slots.push_back(slot);
			}
		}
	}
	
}

void Inventory::addItem(size_t id, int number)
{
	for (auto& item : items)
	{
		if (item)
		{
			//Stack two items if they are the same and can stack
			if (item->id == id && item_manager->getProperties(id).can_stack)
			{
				item->stack_number += number;
				return;
			}
		}
	}

	//Otherwise put the item in an empty slot
	if (!free_slots.empty())
	{
		auto free_slot = findFreeSlot();
		if(free_slot) items[*free_slot] = {id, number};
	}
}

void Inventory::removeItem(int slot)
{
	//Remove the item and free the slot
	if (slot > 0 && slot < items.size())
	{
		items[slot] = std::nullopt;
		free_slots.push_back(slot);
	}
}

void Inventory::splitItemTo(int item_slot, int split_slot)
{
	auto& item = items[item_slot];
	auto& split = items[split_slot];

	if (item && item->stack_number > 1)
	{
		//If items are the same move one item 
		if (split == item)
		{
			item->stack_number--;
			split->stack_number++;
		}
		//If slot is empty than copy item
		else if (!split)
		{
			item->stack_number--;

			split = item;
			split->stack_number = 1;

			std::erase_if(free_slots, [split_slot](int x) { return x == split_slot; });
		}
	}
}

void Inventory::stackItems(int old_item, int new_item)
{
	auto& item = items[old_item];
	auto& item_new = items[new_item];

	if (item && item_new)
	{
		item_new->stack_number += item->stack_number;
		item = std::nullopt;

		free_slots.push_back(old_item);
	}
}

void Inventory::moveItem(int old_slot, int new_slot)
{
	
	std::erase_if(free_slots, [new_slot](int x) { return x == new_slot; });

	if(!items[new_slot]) free_slots.push_back(old_slot);

	std::swap(items[old_slot], items[new_slot]);

	
}

const std::vector<std::optional<Item>>& Inventory::getItems() const
{
	return items;
}

void Inventory::printContent()
{
	int i = 0;
	for (auto& item : items)
	{
		if (item)
		{
			Item& item_ = *item;
			const auto& properties = item_manager->getProperties(item_.id);
			std::cout << "Slot<" << i << "> item: " << properties.name << " " << item_.stack_number << std::endl;
		}
		else
		{
			std::cout << "Slot<" << i << "> is empty" << std::endl;
		}
		++i;
	}
}

std::shared_ptr<ItemManager> Inventory::getItemManager() const
{
	return item_manager;
}

std::shared_ptr<ItemUsageSystem> Inventory::getItemUsageSystem() const
{
	return item_usage_system;
}

std::optional<int> Inventory::findFreeSlot()
{
	if (free_slots.size() <= 0) return std::nullopt;

	int min = std::numeric_limits<int>::max();
	for (int i = 0; i < free_slots.size(); i++)
	{
		if (free_slots[i] < min) min = free_slots[i];
	}

	std::erase_if(free_slots, [min](int value) {return value == min; });
	return min;
}
