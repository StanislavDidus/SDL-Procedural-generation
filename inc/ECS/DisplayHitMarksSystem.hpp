#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "Renderer.hpp"

class DisplayHitMarksSystem
{
public:
	DisplayHitMarksSystem(entt::registry& registry) : registry{registry} {}
	~DisplayHitMarksSystem() = default;

	void update(float dt) const
	{

		auto view = registry.view<Components::HitMark, Components::Renderable>();
		for (auto [entity, hit_mark_component, renderable_component] : view.each())
		{
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

private:
	entt::registry& registry;
};
