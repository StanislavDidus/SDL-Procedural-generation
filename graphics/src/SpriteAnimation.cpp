#include "SpriteAnimation.hpp"

using namespace graphics;

SpriteAnimation::SpriteAnimation(std::shared_ptr<SpriteSheet> sprite_sheet, float frame_rate, const std::vector<int>& frames)
	: sprite_sheet(sprite_sheet)
	, frame_rate(frame_rate)
	, frames(frames)
	, timer(0.0f)
{

}

void SpriteAnimation::update(float dt)
{
	timer += dt;
}

const Sprite& SpriteAnimation::get() const
{
	size_t frame = static_cast<size_t>(timer * frame_rate);
	size_t sprite_index = frame % frames.size();
	return (*sprite_sheet)[frames[sprite_index]];
}
