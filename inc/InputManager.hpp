#pragma once

#include <SDL3/SDL.h>

#include <bitset>

class InputManager
{
public:
	InputManager() = default;

	static bool isKeyDown(SDL_Keycode key);
	static bool isKey(SDL_Keycode key);
	static bool isKeyUp(SDL_Keycode key);

	void buttonUp(SDL_Keycode key);
	void buttonPressed(SDL_Keycode key);

	void update();
	
private:
	std::bitset<256> keys;
	static std::bitset<256> pressed;
	static std::bitset<256> held;
	static std::bitset<256> released;
};

