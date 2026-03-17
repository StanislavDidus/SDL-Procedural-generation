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

		return;
		if (timer >= 10.0f)
		{
			enemy_spawn_system->setMaxEnemy(3);
			enemy_spawn_system->setEnemySpawnTime(0.5f);
		}
	}

private:
	std::shared_ptr<EnemySpawnSystem> enemy_spawn_system;
	float timer = 0.0f;
};
