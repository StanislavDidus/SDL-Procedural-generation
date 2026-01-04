#pragma once

#include "Item.hpp"
#include <optional>
#include <vector>
#include "ECS/Systems.hpp"

#include "ItemManager.hpp"

class Renderer;

class Inventory
{
public:
	Inventory(
		std::shared_ptr<ItemUsageSystem> item_usage_system, 
		std::shared_ptr<ItemManager> item_manager,
		int size = 0
	);

	void useItem(int slot);

	void addItem(int id, int number);
	void removeItem(int slot);

	void splitItemTo(int item_slot, int split_slot);
	void stackItems(int old_item, int new_item);
	void moveItem(int old_slot, int new_slot);

	const std::vector<std::optional<Item>>& getItems() const;
	void printContent();

	void render(Renderer& screen);

	//Getters
	std::shared_ptr<ItemManager> getItemManager() const;
	std::shared_ptr<ItemUsageSystem> getItemUsageSystem() const;

	std::vector<int> free_slots;
private:
	std::optional<int> findFreeSlot();

	std::shared_ptr<ItemUsageSystem> item_usage_system;
	std::shared_ptr<ItemManager> item_manager;
	std::vector<std::optional<Item>> items;

	int size;
};