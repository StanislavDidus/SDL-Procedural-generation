#pragma once

#include <memory>

#include "SDL3/SDL_render.h"

class Sprite
{
public:
	explicit Sprite(std::shared_ptr<SDL_Texture> texture = nullptr, const SDL_FRect& rect = {});

	//Getters
	const SDL_FRect& getRect() const;
	std::shared_ptr<SDL_Texture> getTexture() const;

private:
	std::shared_ptr<SDL_Texture> texture;
	SDL_FRect rect;
};
