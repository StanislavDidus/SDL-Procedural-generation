#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "Item.hpp"

class ItemManager
{
public:

	static ItemManager& get()
	{
		static ItemManager s;
		return s;
	}

	void loadXml(const std::filesystem::path& path);

	//Getters
	const ItemProperties& getItem(size_t ID) const;
	size_t getItemID(const std::string& item_name) const;
	const ItemProperties& getProperties(int ID) const; ///< Get <b>ItemProperties</b> of an item by their <b>ID</b>.
private:
	ItemManager() = default;

	ItemManager(const ItemManager& other) = delete;
	ItemManager(ItemManager&& other) noexcept = delete;
	ItemManager& operator=(const ItemManager& other) = delete;
	ItemManager& operator=(ItemManager&& other) noexcept = delete;

	size_t registerItemProperties(ItemProperties properties);

	std::vector<ItemProperties> items;
	std::unordered_map<std::string, size_t> itemNameToID; ///< Map that returns an <b>ID</b> of an item with the specified name.
	size_t items_counter = 0;
};