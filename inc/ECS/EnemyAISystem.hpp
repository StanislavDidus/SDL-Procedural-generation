#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"

class EnemyAISystem
{
public:
	EnemyAISystem(entt::registry& registry) : registry{registry} {}

	void update(float dt, Entity target_entity)
	{

		if (!registry.all_of<Components::Transform>(target_entity)) return;
		auto& target_transform_component = registry.get<Components::Transform>(target_entity);

		auto view = registry.view <Components::Transform, Components::Physics, Components::EnemyAI, Components::Jump>();
		for (auto [entity, transform_component, physics_component, ai_component, jump_component] : view.each())
		{
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
				jump_component.jump_held = true;
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
	entt::registry& registry;
};