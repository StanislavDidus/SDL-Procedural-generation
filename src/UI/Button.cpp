#include "UI/Button.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"

Button::Button(const glm::vec2& position, const glm::vec2& size)
	: UIElement(position, size)
{

}

void Button::update()
{
	const auto& mouse_state = InputManager::getMouseState();

	SDL_FRect rect{ position.x, position.y, size.x, size.y };
	//bool is_mouse_on_button = isMouseIntersection(mouse_state.position, rect);

	//else if (is_mouse)
}
