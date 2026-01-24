#pragma once

#include "Item.hpp"
#include <optional>
#include <vector>


#include "ItemManager.hpp"

class ItemUsageSystem;

class Renderer;

/// <summary>
/// Stores a certain number of items and allows you to manipulate the items inside by using several public functions. <br>
/// </summary>
class Inventory
{
public:
	Inventory(
		std::shared_ptr<ItemUsageSystem> item_usage_system, 
		int size = 0
	);

	/// <summary>
	/// Use item using specified <b>ItemUsageSystem</b>.
	/// </summary>
	/// <param name="slot">Slot where the item you want to use is placed(starting from 0).</param>
	void useItem(int slot);

	/// <summary>
	/// Adds a certain number of items in any free slot.
	/// </summary>
	/// <param name="id">ID of the item.</param>
	/// <param name="number">Quantity.</param>
	void addItem(size_t id, int number);

	/// <summary>
	/// Clears the given item slot and makes it empty.
	/// </summary>
	/// <param name="slot">Slot index.</param>
	void removeItemAtSlot(int slot);

	/// <summary>
	/// Takes 1 item from the first slot and moves it to the second.
	/// </summary>
	/// <param name="item_slot">Slot you want to split item from.</param>
	/// <param name="split_slot">Slot you want to split item to.</param>
	void splitItemTo(int item_slot, int split_slot);

	/// <summary>
	/// Stacks items in two slots in one.
	/// </summary>
	/// <param name="old_item">First slot.</param>
	/// <param name="new_item">Slot you want those items to be stacked in.</param>
	void stackItems(int old_item, int new_item);

	/// <summary>
	/// Move item from one slot to another(doesn't stack them but swaps instead).
	/// </summary>
	/// <param name="old_slot">Slot of the first item.</param>
	/// <param name="new_slot">Slot of the second item.</param>
	void moveItem(int old_slot, int new_slot);

	/// <summary>
	/// Returns true if item is inside the inventory is specified quantities.
	/// </summary>
	/// <param name="item">Item you need to find.</param>
	/// <returns>True if item is found.</returns>
	bool hasItem(const Item& item) const;

	/// <summary>
	/// Removes given number of items (Removes them even if they are in different slots).
	/// </summary>
	/// <param name="item">Item you want to delete and the number.</param>
	void removeItem(const Item& item);

	/// <summary>
	/// Returns all inventory slots.
	/// </summary>
	/// <returns></returns>
	const std::vector<std::optional<Item>>& getItems() const;
	void printContent();

	//Getters
	std::shared_ptr<ItemUsageSystem> getItemUsageSystem() const;

	
private:
	std::vector<int> free_slots;
	std::optional<int> findFreeSlot();

	std::shared_ptr<ItemUsageSystem> item_usage_system;
	std::vector<std::optional<Item>> items;

	int size;
};