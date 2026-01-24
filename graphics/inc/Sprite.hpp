#pragma once

#include "SDL3/SDL_render.h"

class Sprite
{
public:
	Sprite(SDL_Texture& texture, const SDL_FRect& rect);

	//Getters
	const SDL_FRect& getRect() const;
	SDL_Texture* getTexture() const;

private:
	SDL_Texture& texture;
	SDL_FRect rect;
};
