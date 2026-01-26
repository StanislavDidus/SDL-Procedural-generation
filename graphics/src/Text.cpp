#include <iostream>

#include "Text.hpp"

#include "Color.hpp"
#include "Renderer.hpp"
#include "Surface.hpp"

Text::Text(const Font* font, const Renderer& renderer, const std::string& text, SDL_Color color) : text(text), font(font), color(color)
{
	generateTextTexture(renderer);
}

Text::~Text()
{
	if (texture)
		SDL_DestroyTexture(texture);
}

SDL_Texture* Text::getTexture() const
{
	return texture;
}

void Text::setFont(const Font* font)
{
	this->font = font;
	is_dirty = true;
}

void Text::setColor(SDL_Color color)
{
	this->color = color;
	is_dirty = true;
}

void Text::setText(const std::string& text)
{
	this->text = text;
	is_dirty = true;
}

void Text::updateText(const Renderer& renderer)
{
	//If current text is equal to new text then we don't do anything
	if (!is_dirty) return;

	//If not then we generate new texture
	generateTextTexture(renderer);

	is_dirty = false;
}

void Text::loadTexture(const Renderer& renderer, const Surface& surface)
{
	SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR;
	texture = SDL_CreateTextureFromSurface(renderer.getRenderer(), surface.getSurface());

	SDL_SetTextureScaleMode(texture, scale_mode);

	if (!texture)
	{
		std::cerr << "ERROR: Could not load a texture: " << SDL_GetError() << std::endl;
	}
}

void Text::generateTextTexture(const Renderer& renderer)
{
	if (texture)
		SDL_DestroyTexture(texture);

	Surface surface{ font->getFont(), text, color};
	
	loadTexture(renderer, surface);
}
