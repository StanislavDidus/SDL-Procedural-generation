#include "Inventory.hpp"
#include <iostream>

Inventory::Inventory(int size) : size(size)
{
	items.resize(size);

	for (int i = 0; i < size; i++)
	{
		free_slots.push_back(i);
	}
}

void Inventory::useItem(int slot)
{
	auto& item = items[slot];
	if(item)
	{ 
		/*bool is_usable = false;
		for (const auto& component : item->components)
		{
			if (std::dynamic_pointer_cast<ItemComponents::Usable>(component))
			{
				is_usable = true;
				break;
			}
		}

		if (!is_usable) return;*/

		//item_usage_system.addItem(item);

		item->properties.stack_number--;

		if (item->properties.stack_number <= 0)
		{
			item = std::nullopt;
			free_slots.push_back(slot);
		}
	}
	
}

void Inventory::addItem(const Item& item)
{
	for (auto& i : items)
	{
		if (i)
		{
			Item& item_ = *i;
			if (item_ == item && item_.properties.can_stack)
			{
				item_.properties.stack_number += item.properties.stack_number;
				return;
			}
		}
	}

	if (free_slots.size() > 0)
	{
		int free_slot = free_slots.back();
		free_slots.pop_back();
		items[free_slot] = item;
	}
}

void Inventory::removeItem(int slot)
{
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

	if (item && item->properties.stack_number > 1)
	{
		if (split == item)
		{
			item->properties.stack_number--;

			split->properties.stack_number++;
		}
		else if (!split)
		{
			item->properties.stack_number--;

			split = item;
			split->properties.stack_number = 1;

			std::erase_if(free_slots, [split_slot](int x) { return x == split_slot; });
		}
	}
}

void Inventory::stackItems(int old_item, int new_item)
{
	auto& item = items[old_item];
	auto& item_new = items[new_item];

	item_new->properties.stack_number += item->properties.stack_number;
	item = std::nullopt;

	free_slots.push_back(old_item);
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
			std::cout << "Slot<" << i << "> item: " << item_.properties.name << " " << item_.properties.stack_number << std::endl;
		}
		else
		{
			std::cout << "Slot<" << i << "> is empty" << std::endl;
		}
		++i;
	}
}
