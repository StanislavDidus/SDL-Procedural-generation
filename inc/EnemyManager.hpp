#pragma once

#include <filesystem>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "tinyxml2.h"
#include "ECS/Components.hpp"
#include "ECS/EntityManager.hpp"
#include "ECS/Entity.hpp"

struct Tile;
class TileManager;
class ItemManager;
struct RandomizedItem;
class EnemyManager;


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

	Entity createEnemy(entt::registry& registry, size_t id) const;

	//Getters
	const Components::Enemies::EnemyData& getEnemyData(size_t id) const;
	const Components::Enemies::EnemySpawnInfo& getEnemySpawnInfo(size_t id) const;

	void loadXml(entt::registry& registry, const std::filesystem::path& path);

	const std::vector<Components::Enemies::EnemySpawnInfo>& getAllEnemySpawnInfo() const;

	//std::optional<Entity> createEnemy(const std::string& name) const;
private:
	EnemyManager() = default;

	Entity registerEnemy(entt::registry& registry, const Components::Enemies::EnemyData& enemy_data, const Components::Enemies::EnemySpawnInfo& enemy_spawn_info);

	size_t enemy_count = 0;
	std::vector<Entity> enemies;
	std::vector<Components::Enemies::EnemyData> enemy_datas;
	std::vector<Components::Enemies::EnemySpawnInfo> enemy_spawn_infos;
	//std::unordered_map<std::string, Entity>
};
