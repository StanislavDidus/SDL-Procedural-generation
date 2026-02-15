#include "ColorModGuard.hpp"

namespace graphics
{
	ColorModGuard::ColorModGuard(const std::shared_ptr<SDL_Texture>& texture, const Color& new_color) : texture(texture)
	{
		SDL_GetTextureColorMod(texture.get(), &old_color.r, &old_color.g, &old_color.b);
		SDL_SetTextureColorMod(texture.get(), new_color.r, new_color.g, new_color.b);
	}

	ColorModGuard::~ColorModGuard()
	{
		SDL_SetTextureColorMod(texture.get(), old_color.r, old_color.g, old_color.b);
	}
} // namespace graphics