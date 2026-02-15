#pragma once
#include <memory>

#include "Color.hpp"
#include "SDL3/SDL_render.h"

namespace graphics
{
	/// <summary>
	/// RAII Class that sets new TextureColorMod and then resets it to the previous one when it is no longer needed.
	/// </summary>
	class ColorModGuard
	{
	public:
		ColorModGuard(const std::shared_ptr<SDL_Texture>& texture, const Color& new_color);
		~ColorModGuard();

		ColorModGuard(const ColorModGuard& other) = delete;
		ColorModGuard(ColorModGuard&& other) noexcept = delete;
		ColorModGuard& operator=(const ColorModGuard& other) = delete;
		ColorModGuard& operator=(ColorModGuard&& other) noexcept = delete;

	private:
		std::shared_ptr<SDL_Texture> texture;
		Color old_color;
	};
} // namespace graphics