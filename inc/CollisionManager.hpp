#pragma once

#include <SDL3/SDL.h>
#include "glm/vec2.hpp"

#include <vector>

struct CollisionRect
{
	glm::vec2 position;
	glm::vec2 size;
};

class CollisionManager
{
public:
	CollisionManager() = default;

	void resolveCollision(glm::vec2& position, glm::vec2& size, glm::vec2 velocity) const;
private:
	std::vector<int> collisions;
};