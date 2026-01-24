#include "ObjectManager.hpp"
#include "tinyxml2.h"
#include "ItemManager.hpp"
#include "TileManager.hpp"

void ObjectManager::loadXml(const std::filesystem::path& path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.string().c_str());

	if (doc.Error()) return;

	//Get object listing node
	const auto& object_listing_node = doc.FirstChildElement("objectListing");

	//Loop through all object nodes
	for (auto* object_node = object_listing_node->FirstChildElement("object"); object_node != nullptr; object_node = object_node->NextSiblingElement())
	{
		//Get properties
		const auto& properties_node = object_node->FirstChildElement("properties");

		const char* id = object_node->Attribute("id");
		std::string object_name = id;

		int sprite_index;
		properties_node->FirstChildElement("sprite")->QueryIntText(&sprite_index);

		float durability;
		properties_node->FirstChildElement("durability")->QueryFloatText(&durability);

		std::vector<RandomizedItem> drop;
		const auto& drop_node = properties_node->FirstChildElement("drop");
		for (auto* item_node = drop_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->NextSiblingElement())
		{
			std::string item_name = item_node->Attribute("ref");
			size_t item_id = ItemManager::get().getItemID(item_name);

			float drop_chance;
			item_node->QueryFloatAttribute("dropChance", &drop_chance);

			int drop_quantity_min;
			item_node->QueryIntAttribute("dropQuantityMin", &drop_quantity_min);

			int drop_quantity_max;
			item_node->QueryIntAttribute("dropQuantityMax", &drop_quantity_max);

			drop.emplace_back(item_id, drop_chance, drop_quantity_min, drop_quantity_max);
		}

		ObjectProperties properties{ durability, sprite_index, object_name, drop };

		size_t properties_id = registerObjectProperties(properties);

		//Get spawn info
		const auto& spawn_info_node = object_node->FirstChildElement("spawnInfo");

		const auto& size_node = spawn_info_node->FirstChildElement("size");
		glm::ivec2 size;
		size_node->QueryIntAttribute("x", &size.x);
		size_node->QueryIntAttribute("y", &size.y);

		const auto& noise_settings_node = spawn_info_node->FirstChildElement("noiseSettings");
		NoiseSettings noise_settings;
		noise_settings_node->QueryIntAttribute("octaves", &noise_settings.octaves);
		noise_settings_node->QueryFloatAttribute("frequency", &noise_settings.frequency);
		noise_settings_node->QueryFloatAttribute("amplitude", &noise_settings.amplitude);
		
		float noise_threshold;
		spawn_info_node->FirstChildElement("noiseThreshold")->QueryFloatText(&noise_threshold);

		std::vector<int> spawn_tiles;
		const auto& spawn_tiles_node = spawn_info_node->FirstChildElement("spawnTiles");
		for (auto* tile_node = spawn_tiles_node->FirstChildElement("tile"); tile_node != nullptr; tile_node = tile_node->NextSiblingElement())
		{
			const char* tile_name = tile_node->Attribute("ref");
			size_t tile_id = TileManager::get().getTileID(tile_name);
			spawn_tiles.push_back(tile_id);
		}

		ObjectSpawnInfo spawn_info {spawn_tiles, noise_settings, noise_threshold, size, static_cast<int>(properties_id)};

		addObjectSpawnInfo(spawn_info);
	}
}

const ObjectProperties& ObjectManager::getProperties(int ID) const
{
	return object_properties.at(ID);
}

size_t ObjectManager::registerObjectProperties(const ObjectProperties& properties)
{
	object_properties.push_back(properties);
	return objects_count++;
}

std::vector<ObjectSpawnInfo>& ObjectManager::getAllObjectSpawnInfos()
{
	return object_spawn_infos;
}

void ObjectManager::addObjectSpawnInfo(const ObjectSpawnInfo& spawn_info)
{
	object_spawn_infos.push_back(spawn_info);
}
