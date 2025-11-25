#include "Window.hpp"
#include <iostream>

Window::Window(const char* title, int width, int height, SDL_WindowFlags flags)
{
	window = SDL_CreateWindow(title, width, height, flags);
	
	std::cout << "Window was created" << std::endl;

	SDL_RaiseWindow(window);
	//SDL_SetWindowAlwaysOnTop(window, true);

	SDL_GetWindowSize(window, &window_size.x, &window_size.y);
}

Window::~Window()
{;
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
		case SDL_EVENT_WINDOW_RESIZED:
			SDL_GetWindowSize(window, &window_size.x, &window_size.y);
			break;
		}
	}

	return return_value;
}

const glm::ivec2& Window::getSize() const
{
	return window_size;
}

SDL_Window* Window::getWindow() const
{
	return window;
}

