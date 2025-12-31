#include "UI/FillBar.hpp"

void FillBar::update()
{
	fill_percentage = value / max_value;
	fill_percentage = std::max(0.f, fill_percentage);
}

void FillBar::render(Renderer& screen)
{
	glm::vec2 updated_size;
	if(horizonal) updated_size = glm::vec2{size.x * fill_percentage, size.y};
	else updated_size = glm::vec2{ size.x, size.y * fill_percentage };

	screen.drawRectangle(position.x, position.y, updated_size.x, updated_size.y, RenderType::FILL, color, IGNORE_VIEW_ZOOM);
}
