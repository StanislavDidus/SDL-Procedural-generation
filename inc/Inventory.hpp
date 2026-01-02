#pragma once

#include "Item.hpp"
#include <optional>
#include <vector>

class Renderer;

class Inventory
{
public:
	Inventory(int size = 0);

	void useItem(int slot);

	void addItem(const Item& item);
	void removeItem(int slot);

	void splitItemTo(int item_slot, int split_slot);
	void stackItems(int old_item, int new_item);
	void moveItem(int old_slot, int new_slot);

	const std::vector<std::optional<Item>>& getItems() const;
	void printContent();

	void render(Renderer& screen);

	std::vector<int> free_slots;
private:
	
	std::vector<std::optional<Item>> items;

	int size;
};