#pragma once

#include <entt/entity/registry.hpp>

#include "Components.hpp"

class WindowManagerSystem
{
public:
	WindowManagerSystem(entt::registry& registry) : registry{registry} {}

	void update()
	{
		
	}
private:
	entt::registry& registry;
};