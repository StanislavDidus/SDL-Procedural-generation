#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "Renderer.hpp"
#include "Surface.hpp"

struct Text
{
	std::string text;
	SDL_Texture* texture;
};