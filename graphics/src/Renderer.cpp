#include "Renderer.hpp"
#include "Sprite.hpp"
#include <iostream>

const Color Color::BLACK(0, 0, 0);
const Color Color::RED{ 255,0,0 };
const Color Color::GREEN{ 0,255,0 };
const Color Color::BLUE{ 0,0,255 };
const Color Color::YELLOW(Color::RED + Color::GREEN);

Renderer::Renderer(Window& window) : window(window)
{
	renderer = SDL_CreateRenderer(window.getWindow(), nullptr);

	std::cout << "Renderer was created" << std::endl;
}

Renderer::~Renderer()
{
	SDL_DestroyRenderer(renderer);

	std::cout << "Renderer was deleted" << std::endl;
}

void Renderer::setView(const glm::vec2& view_position)
{
	this->view_position = view_position;
}

SDL_Renderer* Renderer::getRenderer() const
{
	return renderer;
}

const glm::ivec2& Renderer::getWindowSize() const
{
	return window.getSize();
}

void Renderer::clear(Color color)
{
	setColor(color);

	SDL_RenderClear(renderer);
}

void Renderer::update(float dt)
{
	SDL_RenderPresent(renderer);
}

void Renderer::drawPoint(int x, int y, Color color)
{
	drawPoint(static_cast<float>(x), static_cast<float>(y), color);
}

void Renderer::drawPoint(float x, float y, Color color)
{
	setColor(color);
	SDL_RenderPoint(renderer, x - view_position.x, y - view_position.y);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, Color color)
{
	drawLine(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2), color);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, Color color)
{
	setColor(color);
	SDL_RenderLine(renderer, x1 - view_position.x, y1 - view_position.y, x2 - view_position.x, y2 - view_position.y);
}

void Renderer::drawRectangle(int x, int y, int w, int h, RenderType render_type, Color color)
{
	drawRectangle(static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h), render_type, color);
}

void Renderer::drawRectangle(float x, float y, float w, float h, RenderType render_type, Color color)
{
	SDL_FRect rect;
	
	rect.x = x - view_position.x;
	rect.y = y - view_position.y;
	rect.w = w;
	rect.h = h;

	setColor(color);

	switch (render_type)
	{
	case RenderType::NONE:
		SDL_RenderRect(renderer, &rect);
		break;
	case RenderType::FILL:
		SDL_RenderFillRect(renderer, &rect);
		break;
	}
}

void Renderer::drawSprite(const Sprite& sprite, int x, int y)
{
	drawSprite(sprite, static_cast<float>(x), static_cast<float>(y));
}

void Renderer::drawSprite(const Sprite& sprite, float x, float y)
{
	SDL_FRect src, dst;

	src = sprite.getSpriteRect();

	const glm::vec2& texture_size = sprite.getTextureSize();

	dst.x = x - view_position.x;
	dst.y = y - view_position.y;
	dst.w = texture_size.x;
	dst.h = texture_size.y;

	SDL_RenderTexture(renderer, sprite.getTexture(), &src, &dst);
}

void Renderer::drawScaledSprite(const Sprite& sprite, int x, int y, int width, int height)
{
	drawScaledSprite(sprite, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
}

void Renderer::drawScaledSprite(const Sprite& sprite, float x, float y, float width, float height)
{
	SDL_FRect src, dst;

	src = sprite.getSpriteRect();

	dst.x = x - view_position.x;
	dst.y = y - view_position.y;
	dst.w = width;
	dst.h = height;

	SDL_RenderTexture(renderer, sprite.getTexture(), &src, &dst);
}

void Renderer::drawRotatedSprite(const Sprite& sprite, int x, int y, int width, int height, int angle, SDL_FlipMode flip_mode)
{
	drawRotatedSprite(sprite , static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), static_cast<float>(angle), flip_mode);
}

void Renderer::drawRotatedSprite(const Sprite& sprite, float x, float y, float width, float height, float angle, SDL_FlipMode flip_mode)
{
	SDL_FRect src, dst;

	src = sprite.getSpriteRect();
	 
	dst.x = x - view_position.x;
	dst.y = y - view_position.y;
	dst.w = width;
	dst.h = height;

	SDL_RenderTextureRotated(renderer, sprite.getTexture(), &src, &dst, angle, nullptr, flip_mode);
}

void Renderer::drawUI(const Sprite& sprite, float x, float y, float width, float height, float angle, SDL_FlipMode flip_mode)
{
	SDL_FRect src, dst;

	src = sprite.getSpriteRect();

	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;

	if (angle == 0.f || std::fmodf(angle, 360.f) == 0.f)
	{
		SDL_RenderTexture(renderer, sprite.getTexture(), &src, &dst);
	}
	else
	{
		SDL_RenderTextureRotated(renderer, sprite.getTexture(), &src, &dst, angle, nullptr, flip_mode);
	}
}

void Renderer::setColor(Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

