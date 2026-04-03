#include "SpriteSheet.hpp"

#include <iostream>

#include "Surface.hpp"

namespace graphics
{

	SpriteSheet::SpriteSheet(graphics::GpuRenderer& renderer, const std::filesystem::path& path, const SpriteList& sprite_list, SDL_ScaleMode scale_mode)
	{
		texture = renderer.loadTexture(Surface{path});

		for (const auto& sprite : sprite_list)
		{
			sprites.emplace_back(texture, sprite.second);
			nameToIndex[sprite.first] = sprites.size() - 1;
		}
	}

	/*
	SpriteSheet::SpriteSheet(graphics::GpuRenderer& renderer, const Surface& surface, const glm::vec2& sprite_size, SDL_ScaleMode scale_mode)
	{
		auto texture = loadTexture(renderer, surface, scale_mode);

		int texture_width = surface.getSurface()->w;
		int texture_height = surface.getSurface()->h;

		int columns = static_cast<int>(static_cast<float>(texture_width) / sprite_size.x);
		int rows = static_cast<int>(static_cast<float>(texture_height) / sprite_size.y);

		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < columns; x++)
			{
				//sprites.emplace_back(texture, SDL_FRect{ static_cast<float>(x) * sprite_size.x, static_cast<float>(y) * sprite_size.y, sprite_size.x, sprite_size.y });
			}
		}
	}
	*/

	std::shared_ptr<GpuTexture> SpriteSheet::getTexture() const
	{
		return texture;
	}
} // namespace graphics