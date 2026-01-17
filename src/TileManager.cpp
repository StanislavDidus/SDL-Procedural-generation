#include "TileManager.hpp"
#include "tinyxml2.h"

void TileManager::loadXml(const std::filesystem::path& path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.string().c_str());

	if (doc.Error()) return;

	//Get item listing node
	const auto& tile_listing_name = doc.FirstChildElement("tileListing");

	//Loop through all item nodes
	for (auto* tile_node = tile_listing_name->FirstChildElement("tile"); tile_node != nullptr; tile_node = tile_node->NextSiblingElement())
	{
		int sprite_index = tile_node->FirstChildElement("spriteIndex")->IntText();

		bool is_solid = tile_node->FirstChildElement("isSolid")->BoolText();

		float durability = tile_node->FirstChildElement("durability")->FloatText();

		const char* type_name = tile_node->FirstChildElement("tileType")->GetText();

		TileType type = TileType::NONE;
		if (strcmp(type_name, "Surface") == 0)
		{
			type = TileType::SURFACE;
		}
		else if (strcmp(type_name, "Dirt") == 0)
		{
			type = TileType::DIRT;
		}
		else if (strcmp(type_name, "Stone") == 0)
		{
			type = TileType::STONE;
		}
		else if(strcmp(type_name, "Water") == 0)
		{
			type = TileType::WATER;
		}

		std::string tile_name = tile_node->Attribute("id");

		//Register tile
		TileProperties tile_properties{sprite_index, type, is_solid, durability};
		size_t tile_id = registerTile(tile_properties);
		tileNameToID[tile_name] = tile_id;
	}
}

size_t TileManager::getTileID(const std::string& tile_name) const
{
	return tileNameToID.at(tile_name);
}

const TileProperties& TileManager::getProperties(int ID) const
{
	return tiles[ID];
}

size_t TileManager::registerTile(const TileProperties& properties)
{
	tiles.push_back(properties);
	return tiles_count++;
}
