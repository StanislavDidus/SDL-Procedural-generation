#include "Surface.hpp"
#include <iostream>

namespace graphics
{

	Surface::Surface(const std::filesystem::path& path)
	{
		loadImage(path);
	}

	Surface::Surface(const Font& font, const std::string& text, Color color, std::optional<int> wrapped_width)
	{
		loadText(font, text, color, wrapped_width);
	}

	Surface::~Surface()
	{
		if (surface)
			SDL_DestroySurface(surface);
	}

	void Surface::loadImage(const std::filesystem::path& path)
	{
		if (path.extension() == ".png")
		{
			surface = SDL_LoadPNG(path.string().c_str());
		}
		else if (path.extension() == ".bmp")
		{
			surface = SDL_LoadBMP(path.string().c_str());
		}

		if (!surface)
		{
			throw std::runtime_error{ std::format("Could not load image: {}", SDL_GetError()) };
		}

		SDL_PixelFormat format = SDL_PIXELFORMAT_ABGR8888;

		if (format != surface->format)
		{
			SDL_Surface* next = SDL_ConvertSurface(surface, format);

			if (!next)
			{
				throw std::runtime_error{ std::format("Could not convert surface to a desired format: {}", SDL_GetError()) };
			}

			SDL_DestroySurface(surface);
			surface = next;
		}
	}

	void Surface::loadText(const Font& font, const std::string& text, Color color, std::optional<int> wrapped_length)
	{
		if (wrapped_length.has_value())
			surface = TTF_RenderText_Blended_Wrapped(font.getFont(), text.c_str(), text.size(), static_cast<SDL_Color>(color), wrapped_length.value());
		else 
			surface = TTF_RenderText_Blended(font.getFont(), text.c_str(), text.size(), static_cast<SDL_Color>(color));

		if (!surface)
		{
			throw std::runtime_error{ std::format("Could not create a text surface: {}", SDL_GetError()) };
		}
	}

	SDL_Surface* Surface::getSurface() const
	{
		return surface;
	}
} // namespace graphics