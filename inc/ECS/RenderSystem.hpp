#pragma once

#include "Components.hpp"
#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"

#include <queue>

struct RenderEntry
{
	int priority;
	Components::Transform transform;
	Components::Renderable renderable;

	bool operator>(const RenderEntry& rhs) const { return this->priority > rhs.priority; }
};

class RenderSystem
{
public:
	RenderSystem(entt::registry& registry) : registry{registry} {}

	// Update animations
	// NOTE: Move it to another system?
	void update(float dt) const
	{
		auto view = registry.view<Components::Physics, Components::Renderable, Components::CharacterAnimation>(entt::exclude<Components::Dead>);
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
		std::priority_queue<RenderEntry, std::vector<RenderEntry>, std::greater<RenderEntry>> render_queue;

		auto view = registry.view<Components::Transform, Components::Renderable>();
		for (auto [entity, transform_component, renderable_component] : view.each())
		{
			render_queue.push({ renderable_component.priority, transform_component, renderable_component });
		}

		while (render_queue.empty() == false)
		{
			auto& render = render_queue.top();

			const auto& transform_component = render.transform;
			const auto& renderable_component = render.renderable;

			const auto& position = transform_component.position;
			const auto& size = transform_component.size;
			
			graphics::ColorModGuard colorModGuard{renderable_component.sprite.getTexture(), renderable_component.color};

			graphics::drawRotatedSprite(screen, renderable_component.sprite, position.x, position.y, size.x, size.y, 0.f, renderable_component.flip_mode, renderable_component.ignore_view_zoom);

			render_queue.pop();
		}
	}

private:
	entt::registry& registry;
};