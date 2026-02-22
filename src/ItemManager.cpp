#include "ItemManager.hpp"
#include "tinyxml2.h"
#include "ECS/EnttCopyComponents.hpp"

inline int attributeCount(const tinyxml2::XMLElement* e)
{
	int count = 0;
	for (auto* a = e->FirstAttribute(); a != nullptr; a = a->Next())
		++count;
	return count;
}

void ItemManager::loadXml(entt::registry& registry, const std::filesystem::path& path)
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

		auto item = registerItem(registry, { can_stack, sprite_index, item_name, action});

		const auto& components_node = item_node->FirstChildElement("components");
		if (!components_node) continue;
		for (auto* component_node = components_node->FirstChildElement("component"); component_node != nullptr; component_node = component_node->NextSiblingElement())
		{
			const char* component_name = component_node->Attribute("id");
			if (strcmp(component_name, "HealthComponent") == 0)
			{
				float value = component_node->FloatText();
				Components::InventoryItems::HealComponent heal_component{ value };
				registry.emplace_or_replace<Components::InventoryItems::HealComponent>(item, heal_component);
			}
			else if (strcmp(component_name, "PickaxeComponent") == 0)
			{
				float speed = component_node->FloatAttribute("speed");
				float radius = component_node->FloatAttribute("radius");
				int size = component_node->IntAttribute("size");
				Components::InventoryItems::PickaxeComponent pickaxe_component{ speed, radius, size };
				registry.emplace_or_replace<Components::InventoryItems::PickaxeComponent>(item, pickaxe_component);
			}
			else if (strcmp(component_name, "WeaponComponent") == 0)
			{
				float damage = component_node->FloatAttribute("damage");
				float cooldown = component_node->FloatAttribute("cooldown");
				float radius = component_node->FloatAttribute("radius");
				Components::InventoryItems::WeaponComponent weapon_component{ damage, cooldown, radius };
				registry.emplace_or_replace<Components::InventoryItems::WeaponComponent>(item, weapon_component);
			}
			else if (strcmp(component_name, "Boots") == 0)
			{
				registry.emplace_or_replace<Components::InventoryItems::Boots>(item);
			}
			else if (strcmp(component_name, "Helmet") == 0)
			{
				registry.emplace_or_replace<Components::InventoryItems::Helmet>(item);
			}
			else if (strcmp(component_name, "Armor") == 0)
			{
				registry.emplace_or_replace<Components::InventoryItems::Armor>(item);
			}
			else if (strcmp(component_name, "DoubleJump") == 0)
			{
				registry.emplace_or_replace<Components::Effects::DoubleJump>(item);
			}
		}
	}
}


size_t ItemManager::getItemID(const std::string& item_name) const
{
	return itemNameToID.at(item_name);
}

const Components::InventoryItems::ItemProperties& ItemManager::getProperties(int ID) const
{
	return item_properties[ID];
}

const Components::InventoryItems::ItemProperties& ItemManager::getProperties(entt::registry& registry, Entity item) const
{
	const auto& item_info = registry.get<Components::InventoryItems::Item>(item);
	return item_properties[item_info.id];
}

Entity ItemManager::getItem(size_t ID) const
{
	return items[ID];
}

Entity ItemManager::createItem(entt::registry& registry, size_t id, int stack_number) const
{
	const auto& item_origin = items[id];
	auto new_item = registry.create();

	copyComponents(registry, item_origin, new_item);

	registry.get<Components::InventoryItems::Item>(new_item).stack_number = stack_number;

	return new_item;
}

Entity ItemManager::registerItem(entt::registry& registry,
                                 const Components::InventoryItems::ItemProperties& properties)
{
	auto entity = registry.create();
	//registry.emplace<Components::InventoryItems::ItemProperties>(entity, properties);
	registry.emplace<Components::InventoryItems::Item>(entity, Components::InventoryItems::Item{items_counter});
	item_properties.push_back(properties);
	items.push_back(entity);
	itemNameToID[properties.name] = items_counter;
	items_counter++;
	return entity;
}

