#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <filesystem>
#include "Color.hpp"

namespace graphics
{
	class Surface
	{
	public:
		Surface() = default;
		Surface(const std::filesystem::path& path);
		Surface(TTF_Font* font, const std::string& text, Color color = Color::BLACK);
		~Surface();

		bool loadImage(const std::filesystem::path& path);
		bool loadText(TTF_Font* font, const std::string& text, Color color);

		SDL_Surface* getSurface() const;
	private:
		SDL_Surface* surface = nullptr;
	};
} // namespace graphics