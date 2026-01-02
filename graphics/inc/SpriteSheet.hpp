#pragma once

#include <iostream>
#include <vector>

#include "Sprite.hpp"
#include "glm/glm.hpp"
#include "Renderer.hpp"

class SpriteSheet
{
public:
	SpriteSheet(Renderer& renderer, const Surface& surface, std::vector<SDL_FRect> sprite_rects, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
	SpriteSheet(Renderer& renderer, const Surface& surface, const glm::vec2& sprite_size, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
	SpriteSheet(Renderer& renderer, const Surface& surface);
	~SpriteSheet();

	SDL_Texture* getTexture() const;

	const Sprite& operator[](int index) const { return sprites[index]; };
private:
	void loadTexture(Renderer& renderer, const Surface& surface, SDL_ScaleMode scale_mode);

	std::vector<Sprite> sprites;

	SDL_Texture* texture = nullptr;
};