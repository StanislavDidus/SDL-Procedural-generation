#include "EnemyManager.hpp"
#include "ItemManager.hpp"
#include "TileManager.hpp"
#include "RandomizedItem.hpp"
#include "ResourceManager.hpp"
#include "ECS/ComponentManager.hpp"

const EnemyData& EnemyManager::getEnemyData(size_t id) const
{
	return enemy_data[id];
}

const EnemySpawnInfo& EnemyManager::getEnemySpawnInfo(size_t id) const
{
	return enemy_spawn_infos[id];
}

void EnemyManager::loadXml(const std::filesystem::path& path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.string().c_str());

	const auto& enemy_listing_node = doc.FirstChildElement("enemyListing");

	//Loop through sprites
	for (auto* enemy_node = enemy_listing_node->FirstChildElement("enemy"); enemy_node != nullptr; enemy_node = enemy_node->NextSiblingElement())
	{
		const char* name = enemy_node->Attribute("id", nullptr);
		
		glm::ivec2 size{};
		enemy_node->FirstChildElement("size")->QueryIntAttribute("x", &size.x);
		enemy_node->FirstChildElement("size")->QueryIntAttribute("y", &size.y);

		int sprite_index = enemy_node->FirstChildElement("spriteIndex")->IntText();

		bool is_aggressive = enemy_node->FirstChildElement("isAggressive")->BoolText();

		float max_health = enemy_node->FirstChildElement("maxHealth")->FloatText();

		float ai_efficiency = enemy_node->FirstChildElement("aiEfficiency")->FloatText();

		float spawn_chance_weight = enemy_node->FirstChildElement("spawnChanceWeight")->FloatText();

		std::vector<RandomizedItem> items;
		const auto& item_drop_node = enemy_node->FirstChildElement("itemDrop");
		for (auto* item_node = item_drop_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->NextSiblingElement())
		{
			std::string item_name = item_node->Attribute("ref");
			size_t item_id = ItemManager::get().getItemID(item_name);

			float drop_chance;
			item_node->QueryFloatAttribute("dropChance", &drop_chance);

			int drop_quantity_min;
			item_node->QueryIntAttribute("dropQuantityMin", &drop_quantity_min);

			int drop_quantity_max;
			item_node->QueryIntAttribute("dropQuantityMax", &drop_quantity_max);

			items.emplace_back(item_id, drop_chance, drop_quantity_min, drop_quantity_max);
		}

		std::vector<size_t> spawn_tiles;
		const auto& spawn_tiles_node = enemy_node->FirstChildElement("spawnTiles");
		for (auto* tile_node = spawn_tiles_node->FirstChildElement("tile"); tile_node != nullptr; tile_node = tile_node->NextSiblingElement())
		{
			const char* tile_name = tile_node->Attribute("ref");
			size_t tile_id = TileManager::get().getTileID(tile_name);
			spawn_tiles.push_back(tile_id);
		}

		EnemyData enemy_data = EnemyData{ name, sprite_index, is_aggressive, max_health, ai_efficiency, items};
		size_t id = registerEnemy(enemy_data);
		EnemySpawnInfo enemy_spawn_info = EnemySpawnInfo{ id, spawn_chance_weight, size, spawn_tiles};
		addEnemySpawnIfo(enemy_spawn_info);
	}
}

size_t EnemyManager::registerEnemy(const EnemyData& enemy_data)
{
	this->enemy_data.push_back(enemy_data);
	return enemy_count++;
}

void EnemyManager::addEnemySpawnIfo(const EnemySpawnInfo& enemy_spawn_info)
{
	enemy_spawn_infos.push_back(enemy_spawn_info);
}

const std::vector<EnemySpawnInfo>& EnemyManager::getAllEnemySpawnInfo() const
{
	return enemy_spawn_infos;
}

/*
std::optional<Entity> EnemyManager::createEnemy(const std::string& name) const
{
	auto entity = EntityManager::get().createEntity();

	if (!entity) return std::nullopt;

	const auto& enemy_data = enemies_data.at(name);

	auto& component_manager = ComponentManager::get();

	component_manager.renderable[*entity] = Renderable
	{
		(*ResourceManager::get().getSpriteSheet("enemies"))[enemy_data.sprite_index]
	};

	return entity;
}
*/
