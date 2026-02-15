#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"

class EnemyAISystem
{
public:
	EnemyAISystem() = default;

	void update(float dt, Entity target_entity)
	{
		auto& component_manager = ComponentManager::get();

		if (!component_manager.transform.contains(target_entity)) return;
		const auto& target_transform_component = component_manager.transform.at(target_entity);

		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (!component_manager.transform.contains(entity) || !component_manager.physics.contains(entity) || !component_manager.enemy_ai.contains(entity)) continue;

			const auto& transform_component = component_manager.transform.at(entity);
			//std::cout << transform_component.position.x << ", " << transform_component.position.y << std::endl;
			auto& physics_component = component_manager.physics.at(entity);
			auto& ai_component = component_manager.enemy_ai.at(entity);
			auto& jump_component = component_manager.jump.at(entity); 

			//Move Left
			if (target_transform_component.position.x < transform_component.position.x)
			{
				physics_component.velocity.x -= physics_component.acceleration.x * dt;
				physics_component.velocity.x = std::clamp(physics_component.velocity.x, -physics_component.max_velocity.x, physics_component.max_velocity.x);
			}
			//Move right
			else
			{
				physics_component.velocity.x += physics_component.acceleration.x * dt;
				physics_component.velocity.x = std::clamp(physics_component.velocity.x, -physics_component.max_velocity.x, physics_component.max_velocity.x);
			}

			if (ai_component.last_position == transform_component.position)
			{
				jump_component.jump_ready = true;
			}

			//Update AI timer and last position
			ai_component.position_update_timer += dt;
			if (ai_component.position_update_timer >= ai_component.position_update_time)
			{
				ai_component.position_update_timer = 0.0f;
				ai_component.last_position = transform_component.position;
			}
		}
	}

private:

};