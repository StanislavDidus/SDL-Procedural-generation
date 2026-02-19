#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"

class RenderSystem
{
public:
	RenderSystem(entt::registry& registry) : registry{registry} {}

	void update(float dt) const
	{
		auto view = registry.view<Components::Physics, Components::Renderable, Components::CharacterAnimation>();
		for (auto [entity, physics_component, renderable_component, character_animations_component] : view.each())
		{
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
		auto view = registry.view<Components::Transform, Components::Renderable>();
		for (auto [view, transform_component, renderable_component] : view.each())
		{
			const auto& position = transform_component.position;
			const auto& size = transform_component.size;
			
			graphics::ColorModGuard colorModGuard{renderable_component.sprite.getTexture(), renderable_component.color};

			graphics::drawRotatedSprite(screen, renderable_component.sprite, position.x, position.y, size.x, size.y, 0.f, renderable_component.flip_mode);
		}
	}

private:
	entt::registry& registry;
};