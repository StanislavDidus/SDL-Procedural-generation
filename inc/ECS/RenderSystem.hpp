#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"

class RenderSystem
{
public:
	RenderSystem() = default;

	void update(float dt) const
	{
		auto& component_manager = ComponentManager::get();
		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (!component_manager.physics.contains(entity) || !component_manager.renderable.contains(entity) || !component_manager.character_animations.contains(entity)) continue;
			
			const auto& physics_component = component_manager.physics.at(entity);
			auto& renderable_component = component_manager.renderable.at(entity);
			const auto& character_animations_component = component_manager.character_animations.at(entity);

			//First check of jumping and falling
			if (!physics_component.is_ground && physics_component.velocity.y < 100.0f)
			{
				character_animations_component.jump_animation->update(dt);
				renderable_component.sprite = character_animations_component.jump_animation->get();
			}
			else if (!physics_component.is_ground && physics_component.velocity.y >= 100.0f)
			{
				character_animations_component.fall_animation->update(dt);
				renderable_component.sprite = character_animations_component.fall_animation->get();
			}
			//Otherwise, check for idle and walking
			else
			{
				if (std::abs(physics_component.velocity.x) <= 100.f)
				{
					character_animations_component.idle_animation->update(dt);
					renderable_component.sprite = character_animations_component.idle_animation->get();
				}
				else if (std::abs(physics_component.velocity.x) > 100.f)
				{
					character_animations_component.running_animation->update(dt);
					renderable_component.sprite = character_animations_component.running_animation->get();
				}
			}
		}
	}

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
			
			graphics::ColorModGuard colorModGuard{renderable_component.sprite.getTexture(), renderable_component.color};

			graphics::drawRotatedSprite(screen, renderable_component.sprite, position.x, position.y, size.x, size.y, 0.f, renderable_component.flip_mode);
		}
	}
};