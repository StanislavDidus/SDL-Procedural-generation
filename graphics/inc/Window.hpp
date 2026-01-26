#pragma once

#include <SDL3/SDL.h>
#include "glm/glm.hpp"

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

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
	static glm::ivec2 getWindowSize();
	SDL_Window* getWindow() const;

	explicit operator bool() const
	{
		return running;
	}
private:
	bool running = true;

	SDL_Window* window = nullptr;

};

