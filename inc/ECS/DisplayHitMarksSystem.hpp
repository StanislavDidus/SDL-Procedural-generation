#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "Renderer.hpp"

class DisplayHitMarksSystem
{
public:
	DisplayHitMarksSystem() = default;
	~DisplayHitMarksSystem() = default;

	void update(float dt) const
	{
		auto& component_manager = ComponentManager::get();

		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (component_manager.hit_mark.contains(entity) && component_manager.renderable.contains(entity))
			{
				auto& renderable_component = component_manager.renderable.at(entity);
				auto& hit_mark_component = component_manager.hit_mark.at(entity);

				if (!hit_mark_component.active)
				{	
					hit_mark_component.active = true;
					renderable_component.color = graphics::Color::RED;
				}

				hit_mark_component.timer += dt;

				if (hit_mark_component.timer >= hit_mark_component.time)
				{
					renderable_component.color = graphics::Color::WHITE;
				}
			}
		}
	}
};
