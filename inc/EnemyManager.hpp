#pragma once

#include <filesystem>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "tinyxml2.h"
#include "ECS/EntityManager.hpp"

struct Tile;
class TileManager;
class ItemManager;
struct RandomizedItem;
class EnemyManager;

/// <summary>
/// 
/// </summary>
struct EnemyData
{
	std::string name;
	
	int sprite_index;
	bool is_aggressive;
	float max_health;

	//AI settings
	float ai_efficiency;

	std::vector<RandomizedItem> item_drop;
};

/// <summary>
/// 
/// </summary>
struct EnemySpawnInfo
{
	size_t id;
	float spawn_chance_weight;
	glm::ivec2 size;
	std::vector<size_t> spawn_tiles;
};

class EnemyManager
{
public:
	static EnemyManager& get()
	{
		static EnemyManager s;
		return s;
	}

	~EnemyManager() = default;

	EnemyManager(const EnemyManager& other) = delete;
	EnemyManager(EnemyManager&& other) noexcept = delete;
	EnemyManager& operator=(const EnemyManager& other) = delete;
	EnemyManager& operator=(EnemyManager&& other) noexcept = delete;

	//Getters
	const EnemyData& getEnemyData(size_t id) const;
	const EnemySpawnInfo& getEnemySpawnInfo(size_t id) const;

	void loadXml(const std::filesystem::path& path);
	
	size_t registerEnemy(const EnemyData& enemy_data);
	void addEnemySpawnIfo(const EnemySpawnInfo& enemy_spawn_info);

	const std::vector<EnemySpawnInfo>& getAllEnemySpawnInfo() const;

	//std::optional<Entity> createEnemy(const std::string& name) const;
private:
	EnemyManager() = default;

	std::vector<EnemyData> enemy_data;
	std::vector<EnemySpawnInfo> enemy_spawn_infos;
	size_t enemy_count = 0;

	std::unordered_map<std::string, EnemyData> enemies_data;
};
