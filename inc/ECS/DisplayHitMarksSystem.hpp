#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "GpuRenderer.hpp"

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
				hit_mark_component.base_color = renderable_component.color;
				renderable_component.color = graphics::Color::RED;
			}

			hit_mark_component.timer += dt;

			if (hit_mark_component.timer >= hit_mark_component.time)
			{
				renderable_component.color = hit_mark_component.base_color;
				registry.erase<Components::HitMark>(entity);
			}
		}
	}

private:
	entt::registry& registry;
};
