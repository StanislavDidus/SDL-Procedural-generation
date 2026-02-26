#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "ResourceManager.hpp"
#include "Game.hpp"

class DeathSystem
{
public:
	DeathSystem(entt::registry& registry) : registry{registry} {}

	void update(Game& game)
	{
		auto view = registry.view<Components::Health, Components::Renderable, Components::Player>(entt::exclude<Components::Dead>);
		for (auto [entity, health_component, renderable_component] : view.each())
		{
			if (health_component.current_health <= 0.0f)
			{
				registry.emplace<Components::Dead>(entity);
				renderable_component.sprite = (*ResourceManager::get().getSpriteSheet("player"))[9];
				game.setState(GameState::PLAYER_DEAD);
			}
		}
	}
private:
	entt::registry& registry;
};
