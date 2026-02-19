#pragma once

#include "Item.hpp"
#include <optional>
#include <vector>
#include <memory>

#include "ItemManager.hpp"
#include "ECS/Entity.hpp"

class ItemUsageSystem;

namespace graphics
{
	class Renderer;
}

/// <summary>
/// Stores a certain number of items and allows you to manipulate the items inside by using several public functions. <br>
/// </summary>
class Inventory
{
public:
	Inventory(
		int size = 0
	);

	/// <summary>
	/// Use item using specified <b>ItemUsageSystem</b>.
	/// </summary>
	/// <param name="slot">Slot where the item you want to use is placed(starting from 0).</param>
	void useItem(int slot, Entity target_entity, entt::registry& registry);

	//TODO: If I type addItem() with stack that is bigger than 1 with an item that cannot be stacked they WILL be put in one slot.
	/// <summary>
	/// Adds a certain number of items in any free slot. Drops an item if no free place is found.
	/// </summary>
	/// <param name="id">ID of the item.</param>
	/// <param name="number">Quantity.</param> 
	bool addItem(size_t id, int number);
	// Function overloading for addItem(size_t id, int number);
	bool addItem(const Item& item);

	/// <summary>
	/// Clears the given item slot and makes it empty.
	/// </summary>
	/// <param name="slot">Slot index.</param>
	void removeItemAtSlot(size_t slot);

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
	/// Returns the number of items of the specified ID that the inventory contains.
	/// </summary>
	/// <param name="item_id">ID of an item you look for.</param>
	/// <returns>The number of items in the inventory.</returns>
	int countItem(size_t item_id) const;
	
	/// <summary>
	/// Checks if an inventory has at least one free slot.
	/// </summary>
	/// <returns>Return true if there is at least one free slot.</returns>
	bool full() const;

	/// <summary>
	/// Returns all inventory slots.
	/// </summary>
	/// <returns></returns>
	const std::vector<std::unique_ptr<Item>>& getItems() const;

	void printContent() const;
private:
	std::optional<int> findFreeSlot();

	std::vector<int> free_slots;
	std::vector<std::unique_ptr<Item>> items;

	int size;
};