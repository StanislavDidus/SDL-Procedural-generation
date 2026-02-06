#pragma once

#include <SDL3/SDL.h>
#include "glm/vec2.hpp"

namespace graphics
{
class Window
{
public:
	Window(const char* title, int width, int height, SDL_WindowFlags flags);
	~Window();

	Window(const Window& copy) = delete;
	Window& operator=(const Window& copy) = delete;
	Window(const Window&& move) noexcept = delete;
	Window& operator=(Window&& move) noexcept = delete;

	bool pollEvent(SDL_Event& event);

	//Getters
	glm::ivec2 getWindowSize() const;
	SDL_Window* get() const;

	explicit operator bool() const
	{
		return running;
	}
private:
	bool running = true;

	SDL_Window* window = nullptr;

	int width;
	int height;

};
} //namespace graphics