#pragma once
#include "EnemySpawnSystem.hpp"

//Manages enemy spawn rates and their number depending on time
class EnemySpawnManager
{
public:
	EnemySpawnManager(const std::shared_ptr<EnemySpawnSystem>& enemy_spawn_system)
		: enemy_spawn_system(enemy_spawn_system)
	{
		enemy_spawn_system->setEnemiesToSpawn(1);
		enemy_spawn_system->setMaxEnemy(0);
	}

	void update(float dt)
	{
		timer += dt;

		if (timer >= 1000.0f)
		{
			enemy_spawn_system->setEnemySpawnTime(0.1f);
			enemy_spawn_system->setMaxEnemy(21);
			enemy_spawn_system->setEnemiesToSpawn(5);
		}
		else if (timer >= 500.0f) {
			enemy_spawn_system->setEnemySpawnTime(0.3f);
			enemy_spawn_system->setMaxEnemy(15);
		}
		else if (timer >= 350.0f)
		{
			enemy_spawn_system->setMaxEnemy(10);
			enemy_spawn_system->setEnemySpawnTime(0.35f);
			enemy_spawn_system->setEnemiesToSpawn(4);
		}
		else if (timer >= 250.0f) {
			enemy_spawn_system->setMaxEnemy(7);
			enemy_spawn_system->setEnemySpawnTime(0.4f);
		enemy_spawn_system->setEnemiesToSpawn(2);
		}
		else if (timer >= 180.0f)
		{
			enemy_spawn_system->setMaxEnemy(3);
		}
		else if (timer >= 60.0f)
		{
			enemy_spawn_system->setMaxEnemy(1);
			enemy_spawn_system->setEnemySpawnTime(0.5f);
		}
	}

private:
	std::shared_ptr<EnemySpawnSystem> enemy_spawn_system;
	float timer = 0.0f;
};
