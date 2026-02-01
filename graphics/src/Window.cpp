#include "Window.hpp"
#include <iostream>

Window::Window(const char* title, int width, int height, SDL_WindowFlags flags)
{
	window = SDL_CreateWindow(title, width, height, flags);
	
	std::cout << "Window was created" << std::endl;

	SDL_RaiseWindow(window);
	//SDL_SetWindowAlwaysOnTop(window, true);
}

Window::~Window()
{
	SDL_DestroyWindow(window);

	std::cout << "Window was deleted" << std::endl;
}

bool Window::pollEvent(SDL_Event& event)
{
	bool return_value = SDL_PollEvent(&event); 

	if (return_value)
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			running = false;
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			//SDL_GetWindowSize(window, &window_size.x, &window_size.y);
			break;
		case SDL_EVENT_KEY_DOWN:
			if (event.key.key == SDLK_ESCAPE)
				running = false;
			break;
		}
	}

	return return_value;
}

glm::ivec2 Window::getWindowSize()
{
	return {WINDOW_WIDTH, WINDOW_HEIGHT};
}

SDL_Window* Window::get() const
{
	return window;
}

