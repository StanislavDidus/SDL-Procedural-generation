#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"

class RenderSystem
{
public:
	RenderSystem() = default;

	void render(graphics::Renderer& screen) const
	{
		auto& component_manager = ComponentManager::get();
		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (!component_manager.transform.contains(entity) || !component_manager.renderable.contains(entity)) continue;

			const auto& transform_component = component_manager.transform.at(entity);
			const auto& renderable_component = component_manager.renderable.at(entity);

			const auto& position = transform_component.position;
			const auto& size = transform_component.size;
			const auto& sprite = renderable_component.sprite;

			graphics::drawRotatedSprite(screen, sprite, position.x, position.y, size.x, size.y, 0.f, renderable_component.flip_mode);
		}
	}
};