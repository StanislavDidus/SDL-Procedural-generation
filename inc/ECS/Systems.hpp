#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "Renderer.hpp"

#include "InputManager.hpp"

struct PhysicsSystem 
{
	PhysicsSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	void update(float dt) 
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.physics.contains(entity))
			{
				auto& ts = component_manager.transform.at(entity);
				auto& ph = component_manager.physics.at(entity);
				
				ts.position += ph.velocity * dt;
			}
		}
	}
};

struct InputSystem
{
	InputSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	void update(float dt)
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.player.contains(entity) && component_manager.physics.contains(entity))
			{
				auto& p = component_manager.player.at(entity);
				auto& ph = component_manager.physics.at(entity);

				
			}
		}
	}
};