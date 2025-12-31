#include "Inventory.hpp"
#include <iostream>

Inventory::Inventory(int size) : size(size)
{
	items.resize(size);

	for (int i = 0; i < size; i++)
	{
		free_slots.push(i);
	}
}

void Inventory::useItem(int slot)
{
	/*auto& item = items[slot];
	if(item)
	{ 
		bool is_usable = false;
		for (const auto& component : item->components)
		{
			if (std::dynamic_pointer_cast<ItemComponents::Usable>(component))
			{
				is_usable = true;
				break;
			}
		}

		if (!is_usable) return;


	}*/
	
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
		int free_slot = free_slots.front();
		free_slots.pop();
		items[free_slot] = item;
	}
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
