#pragma once

#include "Item.hpp"
#include <optional>
#include <queue>

class Renderer;

class Inventory
{
public:
	Inventory(int size = 0);

	void useItem(int slot);

	void addItem(const Item& item);

	const std::vector<std::optional<Item>>& getItems() const;
	void printContent();

	void render(Renderer& screen);
private:
	std::queue<int> free_slots;
	std::vector<std::optional<Item>> items;

	int size;
};