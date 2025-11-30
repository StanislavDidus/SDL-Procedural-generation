#pragma once

#include <SDL3/SDL.h>
#include <filesystem>

class Surface
{
public:
	Surface() = default;
	Surface(const std::filesystem::path& path);
	~Surface();

	bool loadImage(const std::filesystem::path& path);

	SDL_Surface* getSurface() const;
private:
	SDL_Surface* surface = nullptr;
};