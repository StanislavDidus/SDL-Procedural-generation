#pragma once

#include <vector>
#include "Item.hpp"

/// <summary>
/// Struct that stores an ID of an item that can be crafted by using this recipe
/// and an array of items that are required for the craft.
/// </summary>
struct CraftingRecipe
{
	CraftingRecipe(size_t item_id, const std::vector<Item>& required_items, bool is_blueprint_required);

	size_t item_id; ///< An id of the item that is gonna be crafted.
	std::vector<Item> required_items; ///< Required items for the craft.
	bool is_blueprint_required; ///< Indicates if this crafting recipe is available without a blueprint or not.
};