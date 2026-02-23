#pragma once

#include <UI/Button.hpp>
#include <UI/Button.hpp>
#include <UI/Button.hpp>
#include <UI/Button.hpp>

#include "World.hpp"
#include "ECS/Entity.hpp"
#include <entt/entt.hpp>

struct SpawnRadius
{
	float spawn_minimum_radius = 0.0f;
	float spawn_maximum_radius = 1000.0f;
};

class EnemySpawnSystem
{
public:
	EnemySpawnSystem(entt::registry& registry, const World& world, const SpawnRadius& spawn_radius);

	//Getters
	const std::vector<Entity>& getEnemies() const; 

	void update(float dt, const glm::vec2& target_position, graphics::Renderer& screen); //TODO: Remove Renderer(used for debug purposes).
private:
	Entity createEntity(size_t id) const;
	void destroyEnemy(Entity enemy) const;
	
	void spawnEnemies(const glm::vec2& target_position, int number);

	const World& world;

	SpawnRadius spawn_radius;

	std::vector<Entity> enemies;

	float time = 0.0f;

	float spawn_time = 0.001;
	int max_enemies = 1'000;
	int enemies_to_spawn = 1000;
	float enemy_despawn_distance = 300000000.0f;

	entt::registry& registry;

};