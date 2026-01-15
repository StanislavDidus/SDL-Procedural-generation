#pragma once

/// <summary>
/// A struct that stores an id to the item's properties along with
/// variables that help to randomize the item's drop rate and drop quantities
/// </summary>
struct RandomizedItem
{
	RandomizedItem(const int item_id, const float drop_chance, const int drop_quantity_min, const int drop_quantity_max)
		: item_id(item_id),
		  drop_chance(drop_chance),
		  drop_quantity_min(drop_quantity_min),
		  drop_quantity_max(drop_quantity_max)
	{
	}

	int item_id; ///< Item's properties id.
	float drop_chance; ///< Chance to drop item.
	int drop_quantity_min; ///< Minimum quantity of the item that can drop.
	int drop_quantity_max; ///< Maximum quantity of the item that can drop.
};