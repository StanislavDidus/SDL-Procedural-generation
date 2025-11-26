#pragma once

#include <iostream>
#include <vector>

#include "Sprite.hpp"
#include "glm/glm.hpp"

class SpriteSheet
{
public:
	SpriteSheet(const Surface& surface, std::vector<glm::vec4> sprite_rects);
	SpriteSheet(const Surface& surface, const glm::vec2& sprite_rect);
	~SpriteSheet();
private:
	std::vector<Sprite> sprites;
};