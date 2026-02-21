#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "Item.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EnttCopyComponents.hpp"
#include "ECS/Components.hpp"

class ItemManager
{
public:

	static ItemManager& get()
	{
		static ItemManager s;
		return s;
	}

	void loadXml(entt::registry& registry, const std::filesystem::path& path);

	//Getters
	//const ItemProperties& getItem(size_t ID) const;
	size_t getItemID(const std::string& item_name) const;
	const Components::InventoryItems::ItemProperties& getProperties(int ID) const; ///< Get <b>ItemProperties</b> of an item by their <b>ID</b>.
	const Components::InventoryItems::ItemProperties& getProperties(entt::registry& registry, Entity item) const;
	Entity getItem(size_t ID) const;

	Entity createItem(entt::registry& registry, size_t id, int stack_number = 1) const;
private:
	ItemManager() = default;

	ItemManager(const ItemManager& other) = delete;
	ItemManager(ItemManager&& other) noexcept = delete;
	ItemManager& operator=(const ItemManager& other) = delete;
	ItemManager& operator=(ItemManager&& other) noexcept = delete;

	Entity registerItem(entt::registry& registry, const Components::InventoryItems::ItemProperties& properties);

	std::vector<Entity> items;
	std::vector<Components::InventoryItems::ItemProperties> item_properties;
	std::unordered_map<std::string, size_t> itemNameToID; ///< Map that returns an <b>ID</b> of an item with the specified name.
	size_t items_counter = 0;
};
