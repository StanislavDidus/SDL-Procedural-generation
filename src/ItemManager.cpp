#include "ItemManager.hpp"
#include "tinyxml2.h"

inline int attributeCount(const tinyxml2::XMLElement* e)
{
	int count = 0;
	for (auto* a = e->FirstAttribute(); a != nullptr; a = a->Next())
		++count;
	return count;
}

void ItemManager::loadXml(const std::filesystem::path& path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.string().c_str());

	if (doc.Error()) return;

	//Get item listing node
	const auto& item_listing_node = doc.FirstChildElement("itemListing");

	//Loop through all item nodes
	for (auto* item_node = item_listing_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->NextSiblingElement())
	{
		bool can_stack = item_node->FirstChildElement("canStack")->BoolText();
		
		int sprite_index = item_node->FirstChildElement("spriteIndex")->IntText();

		std::string item_name = item_node->Attribute("id");

		ItemAction action = ItemAction::NONE;

		const char* action_text = item_node->FirstChildElement("action")->GetText();
		if (strcmp(action_text, "USE") == 0)
		{
			action = ItemAction::USE;
		}
		else if (strcmp(action_text, "EQUIP") == 0)
		{
			action = ItemAction::EQUIP;
		}
		else
		{
			action = ItemAction::NONE;
		}

		std::optional<HealData> heal_data;
		std::optional<PickaxeData> pickaxe_data;
		std::optional<MeleeWeaponData> melee_weapon_data;

		const auto& heal_data_node = item_node->FirstChildElement("HealData");
		if (heal_data_node)
		{
			heal_data = HealData{heal_data_node->FloatText()};
		}

		const auto& pickaxe_data_node = item_node->FirstChildElement("PickaxeData");
		if (pickaxe_data_node)
		{
			float speed = pickaxe_data_node->FloatAttribute("speed");
			float radius = pickaxe_data_node->FloatAttribute("radius");
			int size = pickaxe_data_node->IntAttribute("size");
			pickaxe_data = PickaxeData{ speed, radius, size };
		}

		const auto& melee_weapon_data_node = item_node->FirstChildElement("MeleeWeaponData");
		if (melee_weapon_data_node)
		{
			float damage = melee_weapon_data_node->FloatAttribute("damage");
			float cooldown = melee_weapon_data_node->FloatAttribute("cooldown");
			float radius = melee_weapon_data_node->FloatAttribute("radius");
			melee_weapon_data = MeleeWeaponData{ damage, cooldown, radius};
		}

		//Register item
		ItemProperties item_properties {can_stack, sprite_index, item_name, action, heal_data, pickaxe_data, melee_weapon_data};
		size_t item_id = registerItemProperties(item_properties);
		itemNameToID[item_name] = item_id;
	}
}

const ItemProperties& ItemManager::getItem(size_t ID) const
{
	return items.at(ID);
}

size_t ItemManager::getItemID(const std::string& item_name) const
{
	return itemNameToID.at(item_name);
}

const ItemProperties& ItemManager::getProperties(int ID) const
{
	return items[ID];
}

size_t ItemManager::registerItemProperties(const ItemProperties& properties)
{
	items.push_back(properties);
	return items_counter++;
}
