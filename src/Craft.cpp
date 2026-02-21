#include "Craft.hpp"

CraftingRecipe::CraftingRecipe(size_t item_id, const std::vector<Entity>& required_items, bool is_blueprint_required) 
	: item_id(item_id)
	, required_items(required_items)
	, is_blueprint_required(is_blueprint_required)
{
}
