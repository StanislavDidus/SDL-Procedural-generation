#include "InputManager.hpp"

MouseState InputManager::mouse_state{};
std::bitset<256> InputManager::pressed{};
std::bitset<256> InputManager::released{};
std::bitset<256> InputManager::held{};

bool InputManager::isKeyDown(SDL_Keycode key)
{
	return pressed[key];
}

bool InputManager::isKey(SDL_Keycode key)
{
	return held[key];
}

bool InputManager::isKeyUp(SDL_Keycode key)
{
	return released[key];
}

const MouseState& InputManager::getMouseState()
{
	return mouse_state;
}

void InputManager::buttonUp(SDL_Keycode key)
{
	if (key < 256) keys.reset(key);
}

void InputManager::buttonPressed(SDL_Keycode key)
{
	if (key < 256) keys.set(key);
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

void InputManager::update()
{
	pressed = keys & ~held;
	released = held & ~keys;
	held = keys;
}