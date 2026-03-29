#include "Sprite.hpp"

namespace graphics
{

	Sprite::Sprite(std::shared_ptr<GpuTexture> texture, const SDL_FRect& rect) : texture(texture), rect(rect), color{Color::WHITE}
	{

	}

	const SDL_FRect& Sprite::getRect() const
	{
		return rect;
	}

	std::shared_ptr<GpuTexture> Sprite::getTexture() const
	{
		return texture;
	}

	const Color& Sprite::getColor() const
	{
		return color;
	}

	void Sprite::setColor(const Color& color)
	{	
		this->color = color;
	}
} // namespace graphics