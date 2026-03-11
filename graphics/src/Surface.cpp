#include "Surface.hpp"
#include <iostream>

namespace graphics
{

	Surface::Surface(const std::filesystem::path& path)
	{
		loadImage(path);
	}

	Surface::Surface(TTF_Font* font, const std::string& text, Color color, std::optional<int> wrapped_width)
	{
		loadText(font, text, color, wrapped_width);
	}

	Surface::~Surface()
	{
		if (surface)
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

	bool Surface::loadText(TTF_Font* font, const std::string& text, Color color, std::optional<int> wrapped_length)
	{
		if (wrapped_length.has_value())
			surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), text.size(), static_cast<SDL_Color>(color), wrapped_length.value());
		else 
			surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), static_cast<SDL_Color>(color));

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

} // namespace graphics