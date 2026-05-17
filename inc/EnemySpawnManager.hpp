#pragma once
#include "EnemySpawnSystem.hpp"

//Manages enemy spawn rates and their number depending on time
class EnemySpawnManager
{
public:
	EnemySpawnManager(const std::shared_ptr<EnemySpawnSystem>& enemy_spawn_system)
		: enemy_spawn_system(enemy_spawn_system) 
	{}

	void update(float dt)
	{
		timer += dt;

		if (timer >= 1000.0f)
		{
			enemy_spawn_system->setEnemySpawnTime(0.1f);
			enemy_spawn_system->setMaxEnemy(50);
		}
		else if (timer >= 350.0f) {
			enemy_spawn_system->setEnemySpawnTime(0.3f);
			enemy_spawn_system->setMaxEnemy(20);
		}
		else if (timer >= 210.0f)
		{
			enemy_spawn_system->setMaxEnemy(12);
			enemy_spawn_system->setEnemySpawnTime(0.35f);
		}
		else if (timer >= 120.0f) {
			enemy_spawn_system->setMaxEnemy(8);
			enemy_spawn_system->setEnemySpawnTime(0.4f);
		}
		else if (timer >= 60.0f)
		{
			enemy_spawn_system->setMaxEnemy(5);
		}
		else if (timer >= 20.0f)
		{
			enemy_spawn_system->setMaxEnemy(3);
			enemy_spawn_system->setEnemySpawnTime(0.5f);
		}
	}

private:
	std::shared_ptr<EnemySpawnSystem> enemy_spawn_system;
	float timer = 0.0f;
};
