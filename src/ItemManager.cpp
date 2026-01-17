#include "ItemManager.hpp"
#include "tinyxml2.h"

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

		//Get all components
		std::vector<std::unique_ptr<ItemComponent>> item_components;
		const auto& components_node = item_node->FirstChildElement("components");
		for (auto* component_node = components_node->FirstChildElement(); component_node != nullptr; component_node = component_node->NextSiblingElement())
		{
			const auto& component_name = component_node->Name();

			if (strcmp(component_name, "Usable") == 0)
			{
				ItemComponents::Usable usable_component{ };
				item_components.push_back(std::make_unique<ItemComponents::Usable>(usable_component));
			}
			else if (strcmp(component_name, "Heal") == 0)
			{
				int amount;
				component_node->QueryIntAttribute("amount", &amount);

				ItemComponents::Heal heal_component{ amount };
				item_components.push_back(std::make_unique<ItemComponents::Heal>(heal_component));
			}
			else if (strcmp(component_name, "AddEffect") == 0)
			{
				
			}
		}

		//Register item
		ItemProperties item_properties {can_stack, sprite_index, item_name, std::move(item_components)};
		size_t item_id = registerItemProperties(std::move(item_properties));
		itemNameToID[item_name] = item_id;
	}
}

size_t ItemManager::getItemID(const std::string& item_name) const
{
	return itemNameToID.at(item_name);
}

const ItemProperties& ItemManager::getProperties(int ID) const
{
	return items[ID];
}

size_t ItemManager::registerItemProperties(ItemProperties properties)
{
	items.push_back(std::move(properties));
	return items_counter++;
}
