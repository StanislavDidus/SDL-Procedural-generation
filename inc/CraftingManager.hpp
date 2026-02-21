#pragma once

#include "Craft.hpp"
#include <filesystem>
#include "ItemManager.hpp"

class CraftingManager
{
public:

	static CraftingManager& get()
	{
		static CraftingManager s;
		return s;
	}
	

	void loadXml(entt::registry& registry, const std::filesystem::path& path); ///< Takes a <b>path</b> to the <b>XML</b> file and loads the contents of it

	//CraftingRecipe
	size_t getRecipeID(const std::string& recipe_name) const;
	const CraftingRecipe& getRecipe(size_t ID) const;
	const std::vector<CraftingRecipe>& getAllRecipes() const;
	size_t size() const;
private:
	CraftingManager() = default;

	CraftingManager(const CraftingManager& other) = delete;
	CraftingManager(CraftingManager&& other) noexcept = delete;
	CraftingManager& operator=(const CraftingManager& other) = delete;
	CraftingManager& operator=(CraftingManager&& other) noexcept = delete;

	size_t registerRecipe(const CraftingRecipe& recipe); ///< Adds new <b>ObjectProperties</b> to the <b>ObjectManager</b> and returns an ID to them.

	std::vector<CraftingRecipe> recipes;
	std::unordered_map<std::string, size_t> recipeNameToID;

	size_t recipes_count = 0;
};

