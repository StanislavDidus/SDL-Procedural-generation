#include "Sprite.hpp"

Sprite::Sprite(SDL_Texture& texture, const SDL_FRect& rect) : texture(texture), rect(rect)
{
	
}

Sprite::~Sprite()
{
	
}

const SDL_FRect& Sprite::getRect() const
{
	return rect;
}

SDL_Texture* Sprite::getTexture() const
{
	return &texture;
}

