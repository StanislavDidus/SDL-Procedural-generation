#pragma once

#include <memory>
#include <vector>

#include "Sprite.hpp"
#include "glm/glm.hpp"

class Renderer;
class Surface;

using Texture = std::shared_ptr<SDL_Texture>;

class SpriteSheet
{
public:
	SpriteSheet(const Renderer& renderer, const Surface& surface, const std::vector<SDL_FRect>& sprite_rects, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
	SpriteSheet(const Renderer& renderer, const Surface& surface, const glm::vec2& sprite_size, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);

	SpriteSheet(const SpriteSheet& other) = delete;
	SpriteSheet(SpriteSheet&& other) noexcept = delete;
	SpriteSheet& operator=(const SpriteSheet& other) = delete;
	SpriteSheet& operator=(SpriteSheet&& other) noexcept = delete;

	const Sprite& operator[](int index) const { return sprites[index]; };
private:
	Texture loadTexture(const Renderer& renderer, const Surface& surface, SDL_ScaleMode scale_mode);

	std::vector<Sprite> sprites;
};
