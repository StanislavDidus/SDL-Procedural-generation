#pragma once

#include "glm/vec2.hpp"

struct Vec2Less
{
	bool operator()(const glm::ivec2& a, const glm::ivec2& b) const
	{
		if (a.x != b.x) return a.x < b.x;
		return a.y < b.y;
	}

	bool operator()(const glm::vec2& a, const glm::vec2& b) const
	{
		if (a.x != b.x) return a.x < b.x;
		return a.y < b.y;
	}
};