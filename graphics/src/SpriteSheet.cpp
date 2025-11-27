#include "SpriteSheet.hpp"

SpriteSheet::SpriteSheet(Renderer& renderer, const Surface& surface, std::vector<SDL_FRect> sprite_rects, SDL_ScaleMode scale_mode)
{
	texture = SDL_CreateTextureFromSurface(renderer.getRenderer(), surface.getSurface());

	SDL_SetTextureScaleMode(texture, scale_mode);

	if (!texture)
	{
		std::cerr << "ERROR: Could not load a texture: " << SDL_GetError() << std::endl;
	}
	
	for (const auto& rect : sprite_rects)
	{
		sprites.emplace_back(*texture, rect);
	}
}

SpriteSheet::SpriteSheet(Renderer& renderer, const Surface& surface, const glm::vec2& sprite_size, SDL_ScaleMode scale_mode)
{
	texture = SDL_CreateTextureFromSurface(renderer.getRenderer(), surface.getSurface());

	SDL_SetTextureScaleMode(texture, scale_mode);

	if (!texture)
	{
		std::cerr << "ERROR: Could not load a texture: " << SDL_GetError() << std::endl;
	}
	
	int texture_width = surface.getSurface()->w;
	int texture_height = surface.getSurface()->h;

	int columns = static_cast<int>(static_cast<float>(texture_width) / sprite_size.x);
	int rows = static_cast<int>(static_cast<float>(texture_height) / sprite_size.y);

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			sprites.emplace_back(*texture, SDL_FRect{ static_cast<float>(x) * sprite_size.x, static_cast<float>(y) * sprite_size.y, sprite_size.x, sprite_size.y });
		}
	}
}

SpriteSheet::~SpriteSheet()
{
	SDL_DestroyTexture(texture);
}
