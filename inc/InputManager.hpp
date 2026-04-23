#pragma once

#include <SDL3/SDL.h>
#include "glm/vec2.hpp"

#include <bitset>

constexpr size_t BITSET_SIZE = 250;

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

	std::bitset<BITSET_SIZE> keys;
	static std::bitset<BITSET_SIZE> pressed;
	static std::bitset<BITSET_SIZE> held;
	static std::bitset<BITSET_SIZE> released;
};

