#pragma once

#include <SDL3/SDL.h>
#include "glm/vec2.hpp"

#include <bitset>

enum class MouseButtonState
{
	NONE,
	DOWN,
	HELD,
	RELEASED,
};	

struct MouseState
{
	glm::vec2 position;
	MouseButtonState left;
	MouseButtonState right;
	glm::vec2 wheel;
};

class InputManager
{
public:
	InputManager() = default;

	static bool isKeyDown(SDL_Keycode key);
	static bool isKey(SDL_Keycode key);
	static bool isKeyUp(SDL_Keycode key);
	
	static const MouseState& getMouseState();

	void buttonUp(SDL_Keycode key);
	void buttonPressed(SDL_Keycode key);
	void setMouseState(const glm::vec2& position, bool left_down, bool right_down);
	void setMouseWheel(const glm::vec2& wheel);

	void update();
	
private:
	static MouseState mouse_state;

	std::bitset<256> keys;
	static std::bitset<256> pressed;
	static std::bitset<256> held;
	static std::bitset<256> released;
};

