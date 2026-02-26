#include "InputManager.hpp"

MouseState InputManager::mouse_state{};
std::bitset<BITSET_SIZE> InputManager::pressed{};
std::bitset<BITSET_SIZE> InputManager::released{};
std::bitset<BITSET_SIZE> InputManager::held{};

bool InputManager::isKeyDown(SDL_Scancode key)
{
	return pressed[key];
}

bool InputManager::isKey(SDL_Scancode key)
{
	return held[key];
}

bool InputManager::isKeyUp(SDL_Scancode key)
{
	return released[key];

}

const MouseState& InputManager::getMouseState()
{
	return mouse_state;
}

void InputManager::buttonUp(SDL_Scancode key)
{
	if (key < BITSET_SIZE) keys.reset(key);
}

void InputManager::buttonPressed(SDL_Scancode key)
{
	if (key < BITSET_SIZE) keys.set(key);
}

void InputManager::setMouseState(const glm::vec2& position, bool left_down, bool right_down)
{
	mouse_state.position = position;

	switch (mouse_state.left)
	{
	case MouseButtonState::NONE:
		left_down ? mouse_state.left = MouseButtonState::DOWN : mouse_state.left = MouseButtonState::NONE;
		break;
	case MouseButtonState::DOWN:
		left_down ? mouse_state.left = MouseButtonState::HELD : mouse_state.left = MouseButtonState::RELEASED;
		break;
	case MouseButtonState::HELD:
		left_down ? mouse_state.left = MouseButtonState::HELD : mouse_state.left = MouseButtonState::RELEASED;
		break;
	case MouseButtonState::RELEASED:
		left_down ? mouse_state.left = MouseButtonState::DOWN : mouse_state.left = MouseButtonState::NONE;
		break;
	}

	switch (mouse_state.right)
	{
	case MouseButtonState::NONE:
		right_down ? mouse_state.right = MouseButtonState::DOWN : mouse_state.right = MouseButtonState::NONE;
		break;
	case MouseButtonState::DOWN:
		right_down ? mouse_state.right = MouseButtonState::HELD : mouse_state.right = MouseButtonState::RELEASED;
		break;
	case MouseButtonState::HELD:
		right_down ? mouse_state.right = MouseButtonState::HELD : mouse_state.right = MouseButtonState::RELEASED;
		break;
	case MouseButtonState::RELEASED:
		right_down ? mouse_state.right = MouseButtonState::DOWN : mouse_state.right = MouseButtonState::NONE;
		break;
	}
}

void InputManager::setMouseWheel(const glm::vec2& wheel)
{
	mouse_state.wheel = wheel;
}

void InputManager::update()
{
	pressed = keys & ~held;
	released = held & ~keys;
	held = keys;
}
