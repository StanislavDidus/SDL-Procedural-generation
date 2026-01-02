#include "Surface.hpp"
#include <iostream>

Surface::Surface(const std::filesystem::path& path)
{
	loadImage(path);
}

Surface::Surface(TTF_Font* font, const std::string& text, SDL_Color color)
{
	loadText(font, text, color);
}

Surface::~Surface()
{
	SDL_DestroySurface(surface);
}

bool Surface::loadImage(const std::filesystem::path& path)
{
	surface = SDL_LoadPNG(path.string().c_str());

	if (!surface)
	{
		std::cerr << "ERROR: Could not find a png:" << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

bool Surface::loadText(TTF_Font* font, const std::string& text, SDL_Color color)
{
	surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);

	if (!surface)
	{
		std::cerr << "ERROR: Something went wrong:" << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

SDL_Surface* Surface::getSurface() const
{
	return surface;
}
