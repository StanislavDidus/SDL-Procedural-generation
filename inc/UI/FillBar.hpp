#pragma once

#include "Color.hpp"
#include "UI/UIElement.hpp"

class FillBar : public UIElement
{
public:
	FillBar(const glm::vec2& position, const glm::vec2& size, const float& value, float max_value, Color color) : UIElement(position, size), value(value), max_value(max_value), color(color) {}

	void update() override;
	void render(Renderer& screen) override;
private:
	bool horizonal = true; // false - vertical
	const float& value;
	float fill_percentage = 0.f;
	float max_value;
	Color color;
};