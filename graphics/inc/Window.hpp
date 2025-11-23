#pragma once

#include <SDL3/SDL.h>
#include "glm/glm.hpp"

class Window
{
public:
	Window(const char* title, int width, int height, SDL_WindowFlags flags);
	virtual ~Window();

	bool pollEvent(SDL_Event& event);

	//Getters
	const glm::ivec2& getSize() const;
	SDL_Window* getWindow() const;
private:
	SDL_Window* window = nullptr;

	glm::ivec2 window_size {0.f, 0.f};
};

