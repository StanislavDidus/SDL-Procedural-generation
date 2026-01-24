#include "CraftingManager.hpp"
#include "tinyxml2.h"

void CraftingManager::loadXml(const std::filesystem::path& path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.string().c_str());

	const auto& craftListing = doc.FirstChildElement("craftListing");

	for (auto* craft_node = craftListing->FirstChildElement("craft"); craft_node != nullptr; craft_node = craft_node->NextSiblingElement())
	{
		const char* craft_item_name = craft_node->Attribute("id");
		size_t craft_item_id = ItemManager::get().getItemID(craft_item_name);

		bool is_blueprint_required = craft_node->FirstChildElement("requiredBlueprint")->BoolText();

		std::vector<Item> required_items;
		const auto& required_items_node = craft_node->FirstChildElement("requiredItems");
		for (auto* item_node = required_items_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->NextSiblingElement())
		{
			const char* item_name = item_node->Attribute("ref");
			int item_amount = item_node->IntAttribute("amount");

			size_t item_id = ItemManager::get().getItemID(item_name);
			required_items.emplace_back( item_id, item_amount );
		}

		CraftingRecipe recipe{ craft_item_id, required_items, is_blueprint_required };
		size_t recipe_id = registerRecipe(recipe);	
		recipeNameToID[craft_item_name] = recipe_id;
	}
}

size_t CraftingManager::getRecipeID(const std::string& recipe_name) const
{
	return recipeNameToID.at(recipe_name);
}

const CraftingRecipe& CraftingManager::getRecipe(int ID) const
{
	return recipes.at(ID);
}

const std::vector<CraftingRecipe>& CraftingManager::getAllRecipes() const
{
	return recipes;
}

size_t CraftingManager::size() const
{
	return recipes_count;
}

size_t CraftingManager::registerRecipe(const CraftingRecipe& recipe)
{
	recipes.push_back(recipe);
	return recipes_count++;
}
