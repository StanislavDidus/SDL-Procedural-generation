#pragma once

#include "Renderer.hpp"
#include "glm/vec2.hpp"
#include "SDL3/SDL_rect.h"

class UIElement
{
public:
	UIElement(const glm::vec2& position, const glm::vec2& size) : position(position), size(size) {}
	virtual ~UIElement() = default;

	virtual void update() = 0;
	virtual void render(Renderer& screen) = 0;

protected:
	glm::vec2 position;
	glm::vec2 size;
};