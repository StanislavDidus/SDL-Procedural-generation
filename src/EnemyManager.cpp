#include "EnemyManager.hpp"
#include "ItemManager.hpp"
#include "TileManager.hpp"
#include "RandomizedItem.hpp"
#include "ResourceManager.hpp"
#include "ECS/ComponentManager.hpp"
#include "ECS/EnttCopyComponents.hpp"

using namespace Components::Enemies;

Entity EnemyManager::createEnemy(entt::registry& registry, size_t id) const
{
	const auto& enemy_origin = enemies[id];
	auto new_item = registry.create();

	copyComponents(registry, enemy_origin, new_item);

	return new_item;
}

const EnemyData& EnemyManager::getEnemyData(size_t id) const
{
	return enemy_datas[id];
}

const EnemySpawnInfo& EnemyManager::getEnemySpawnInfo(size_t id) const
{
	return enemy_spawn_infos[id];
}

void EnemyManager::loadXml(entt::registry& registry, const std::filesystem::path& path)
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
		EnemySpawnInfo enemy_spawn_info = EnemySpawnInfo{ enemy_count, spawn_chance_weight, size, spawn_tiles};
	
		auto enemy = registerEnemy(registry, enemy_data, enemy_spawn_info);

		const auto& components_node = enemy_node->FirstChildElement("components");
		for (auto* component_node = components_node->FirstChildElement("component"); component_node != nullptr; component_node = component_node->NextSiblingElement())
		{
			const auto* component_name = component_node->Attribute("id");

			if (strcmp(component_name, "EnemyAI") == 0)
			{
				registry.emplace_or_replace<Components::Enemies::EnemyAI>(enemy, 2.0f);
			}
			else if (strcmp(component_name, "DropEssence") == 0)
			{
				EssenceType essence_type = EssenceType::NONE;
				const char* essence_type_name = component_node->Attribute("value");

				if (strcmp(essence_type_name, "Common") == 0) essence_type = EssenceType::COMMON;
				else if (strcmp(essence_type_name, "Snow") == 0) essence_type = EssenceType::SNOW;
				else if (strcmp(essence_type_name, "Sand") == 0) essence_type = EssenceType::SAND;

				int number = 0;
				component_node->QueryIntAttribute("number", &number);

				float chance = 0.0f;
				component_node->QueryFloatAttribute("chance", &chance);

				registry.emplace_or_replace<Components::Enemies::DropEssence>(enemy, essence_type, number, chance);
			}
		}
	}
}

const std::vector<EnemySpawnInfo>& EnemyManager::getAllEnemySpawnInfo() const
{
	return enemy_spawn_infos;
}

Entity EnemyManager::registerEnemy(entt::registry& registry,
                                 const Components::Enemies::EnemyData& enemy_data, const Components::Enemies::EnemySpawnInfo& enemy_spawn_info)
{
	auto entity = registry.create();
	//registry.emplace<Components::InventoryItems::ItemProperties>(target, properties);
	registry.emplace<Components::Enemies::Enemy>(entity, Components::Enemies::Enemy{enemy_count});
	enemy_datas.push_back(enemy_data);
	enemy_spawn_infos.push_back(enemy_spawn_info);
	enemies.push_back(entity);
	//itemNameToID[properties.name] = items_counter;
	enemy_count++;
	return entity;
}
