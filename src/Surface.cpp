#include "Surface.hpp"
#include <iostream>

Surface::Surface(const std::filesystem::path& path)
{
	loadImage(path);
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

SDL_Surface* Surface::getSurface() const
{
	return surface;
}
