#pragma once

#include <SDL3/SDL.h>
#include "Surface.hpp"
#include "Renderer.hpp"
#include "glm/glm.hpp"

class Sprite
{
public:
	Sprite(SDL_Texture& texture, const SDL_FRect& rect);
	virtual ~Sprite();

	//Getters
	const SDL_FRect& getRect() const;
	SDL_Texture* getTexture() const;

private:
	SDL_Texture& texture;
	SDL_FRect rect;
};
