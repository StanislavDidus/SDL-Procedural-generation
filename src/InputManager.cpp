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

void InputManager::setMouseState(const MouseState& mouse_state)
{
	this->mouse_state = mouse_state;
}

void InputManager::update()
{
	pressed = keys & ~held;
	released = held & ~keys;
	held = keys;
}