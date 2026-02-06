#include "Font.hpp"
#include <iostream>

namespace graphics
{

	Font::Font(const std::filesystem::path& path, int size)
	{
		loadFont(path, size);
	}

	Font::~Font()
	{
		TTF_CloseFont(ttf_font);
	}

	bool Font::loadFont(const std::filesystem::path& path, int size)
	{
		ttf_font = TTF_OpenFont(path.string().c_str(), size);

		if (!ttf_font)
		{
			std::cerr << "ERROR: Something went wrong: " << SDL_GetError() << std::endl;
			return false;
		}
		return true;
	}

	TTF_Font* Font::getFont() const
	{
		return ttf_font;
	}

} // namespace graphics