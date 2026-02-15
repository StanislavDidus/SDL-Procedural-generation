#pragma once

#include <memory>

#include "Color.hpp"
#include "SDL3/SDL_render.h"

namespace graphics
{
	class Sprite
	{
	public:
		explicit Sprite(std::shared_ptr<SDL_Texture> texture = nullptr, const SDL_FRect& rect = {});

		//Getters
		const SDL_FRect& getRect() const;
		std::shared_ptr<SDL_Texture> getTexture() const;
		const Color& getColor() const;

		//Setters
		void setColor(const Color& color);

	private:
		std::shared_ptr<SDL_Texture> texture;
		SDL_FRect rect;

		Color color = Color::WHITE;
	};

} // namespace graphics