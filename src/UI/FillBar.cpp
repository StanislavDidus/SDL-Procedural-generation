#include "UI/FillBar.hpp"
#include "RenderFunctions.hpp"

#include <algorithm>

using namespace graphics;

void FillBar::update()
{
	fill_percentage = value / max_value;
	fill_percentage = std::max(0.f, fill_percentage);
}

void FillBar::render(graphics::Renderer& screen)
{
	glm::vec2 updated_size;
	if(horizonal) updated_size = glm::vec2{size.x * fill_percentage, size.y};
	else updated_size = glm::vec2{ size.x, size.y * fill_percentage };

	drawRectangle(screen, position.x, position.y, updated_size.x, updated_size.y, RenderType::FILL, color, IGNORE_VIEW_ZOOM);
}
