#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "Renderer.hpp"

class RenderSystem
{
public:
	RenderSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	void render(Renderer& screen) const
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (!component_manager.transform.contains(entity) || !component_manager.renderable.contains(entity)) continue;

			const auto& transform_component = component_manager.transform.at(entity);
			const auto& renderable_component = component_manager.renderable.at(entity);

			const auto& position = transform_component.position;
			const auto& size = transform_component.size;
			const auto& sprite = renderable_component.sprite;

			screen.drawRotatedSprite(sprite, position.x, position.y, size.x, size.y, 0.f, renderable_component.flip_mode);
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
};