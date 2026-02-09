#pragma once

#include "SpriteSheet.hpp"

#include <vector>
#include <memory>

namespace graphics
{
	class SpriteAnimation
	{
	public:
		SpriteAnimation(std::shared_ptr<SpriteSheet> sprite_sheet, float frame_rate, const std::vector<int>& frames);

		void update(float dt);
		const Sprite& get();
	private:
		std::shared_ptr<SpriteSheet> sprite_sheet;
		float frame_rate;
		std::vector<int> frames;
		float timer = 0.0f;
	};

} // namespace graphics