#pragma once

#include "glm/vec2.hpp"
#include "UIElement.hpp"

class Renderer;

class Button : public UIElement
{
public:
	Button(const glm::vec2& position, const glm::vec2& size);

	void update() override;
private:
		
};