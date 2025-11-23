#include "Sprite.hpp"
#include <iostream>

Sprite::Sprite(Renderer& renderer, const Surface& surface, const glm::vec2& frame_size, SDL_ScaleMode scale_mode) : surface(surface), frame_size(frame_size)
{
	texture = SDL_CreateTextureFromSurface(renderer.getRenderer(), surface.getSurface());

	SDL_SetTextureScaleMode(texture, scale_mode);

	if (!texture)
	{
		std::cerr << "ERROR: Could not load a texture: " << SDL_GetError() << std::endl;
	}

	SDL_GetTextureSize(texture, &texture_size.x, &texture_size.y);

	if (frame_size == glm::vec2{ 0.f, 0.f })
	{
		sprite_rect = { 0.f, 0.f, texture_size.x, texture_size.y };
		max_frames = 1;
	}
	else
	{
		columns = texture_size.x / frame_size.x;
		int raws = texture_size.y / frame_size.y;
		max_frames = raws * columns;

		setSpriteRect();
	}
}

Sprite::~Sprite()
{
	SDL_DestroyTexture(texture);
}

void Sprite::setFrame(int frame)
{
	current_frame = frame;
	current_frame = std::fmodf(current_frame, max_frames);

	setSpriteRect();
}

const SDL_FRect& Sprite::getSpriteRect() const
{
	return sprite_rect;
}

SDL_Texture* Sprite::getTexture() const
{
	return texture;
}

const glm::vec2& Sprite::getTextureSize() const
{
	return texture_size;
}

void Sprite::setSpriteRect()
{
	float x = (current_frame % columns) * frame_size.x;
	float y = (current_frame / columns) * frame_size.y;

	sprite_rect = { x, y, frame_size.x, frame_size.y };
}
