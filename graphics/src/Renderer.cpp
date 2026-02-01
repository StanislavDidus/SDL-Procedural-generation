#include "Renderer.hpp"
#include "Sprite.hpp"
#include "Color.hpp"
#include "Window.hpp"
#include "Text.hpp"

#include <iostream>

Renderer::Renderer(Window& window) : window(window)
{
	renderer = SDL_CreateRenderer(window.get(), nullptr);

	std::cout << "Renderer was created" << std::endl;

	SDL_SetRenderVSync(renderer, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

Renderer::~Renderer()
{
	SDL_DestroyRenderer(renderer);

	std::cout << "Renderer was deleted" << std::endl;
}

void Renderer::setView(const glm::vec2& view_position)
{
	this->view_position = view_position - static_cast<glm::vec2>(getWindowSize()) * 0.5f;
}

void Renderer::setZoom(float zoom)
{
	this->zoom = zoom;
}


const glm::vec2& Renderer::getView() const
{
	return view_position;
}

float Renderer::getZoom() const
{
	return zoom;
}

SDL_Renderer* Renderer::getRenderer() const
{
	return renderer;
}

const glm::ivec2& Renderer::getWindowSize() const
{
	return window.getWindowSize();
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

	float x_ = x - view_position.x;
	float y_ = y - view_position.y;

	x_ *= zoom;
	y_ *= zoom;

	SDL_RenderPoint(renderer, x_, y_);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, Color color)
{
	drawLine(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2), color);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, Color color)
{
	setColor(color);

	float x1_ = x1 - view_position.x;
	float y1_ = y1 - view_position.y;
	float x2_ = x2 - view_position.x;
	float y2_ = y2 - view_position.y;

	x1_ *= zoom;
	y1_ *= zoom;
	x2_ *= zoom;
	y2_ *= zoom;

	SDL_RenderLine(renderer, x1_, y1_, x2_, y2_ );
}

void Renderer::drawRectangle(int x, int y, int w, int h, RenderType render_type, Color color, bool ignore_view_zoom)
{
	drawRectangle(static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h), render_type, color, ignore_view_zoom);
}

void Renderer::drawRectangle(float x, float y, float w, float h, RenderType render_type, Color color, bool ignore_view_zoom)
{
	SDL_FRect rect;
	
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	if (!ignore_view_zoom)
	{
		rect.x -= view_position.x;
		rect.y -= view_position.y;

		zoomRect(rect);
	}

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

	src = sprite.getRect();

	dst.x = x - view_position.x;
	dst.y = y - view_position.y;
	dst.w = src.w;
	dst.h = src.h;

	zoomRect(dst);

	SDL_RenderTexture(renderer, sprite.getTexture().get(), &src, &dst);
}

void Renderer::drawScaledSprite(const Sprite& sprite, int x, int y, int width, int height, bool ignore_view_zoom)
{
	drawScaledSprite(sprite, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
}

void Renderer::drawScaledSprite(const Sprite& sprite, float x, float y, float width, float height, bool ignore_view_zoom)
{
	SDL_FRect src, dst;

	src = sprite.getRect();

	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;

	if (!ignore_view_zoom)
	{
		dst.x -= view_position.x;
		dst.y -= view_position.y;

		zoomRect(dst);
	}

	SDL_RenderTexture(renderer, sprite.getTexture().get(), &src, &dst);
}

void Renderer::drawRotatedSprite(const Sprite& sprite, int x, int y, int width, int height, int angle, SDL_FlipMode flip_mode)
{
	drawRotatedSprite(sprite , static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), static_cast<float>(angle), flip_mode);
}

void Renderer::drawRotatedSprite(const Sprite& sprite, float x, float y, float width, float height, float angle, SDL_FlipMode flip_mode)
{
	SDL_FRect src, dst;

	src = sprite.getRect();
	 
	dst.x = x - view_position.x;
	dst.y = y - view_position.y;
	dst.w = width;
	dst.h = height;

	zoomRect(dst);

	SDL_RenderTextureRotated(renderer, sprite.getTexture().get(), &src, &dst, angle, nullptr, flip_mode);
}

void Renderer::printText(const Text& text, float x, float y, float w, float h, bool ignore_view_zoom)
{
	SDL_FRect rect;

	SDL_Texture* texture = text.getTexture();

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	if (!ignore_view_zoom)
	{
		rect.x -= view_position.x;
		rect.y -= view_position.y;

		zoomRect(rect);
	}

	SDL_RenderTexture(renderer, texture, nullptr, &rect);
}

void Renderer::printTextScaled(const Text& text, float x, float y, float scale_x, float scale_y, bool ignore_view_zoom)
{
	SDL_FRect rect;

	SDL_Texture* texture = text.getTexture();

	rect.x = x;
	rect.y = y;
	rect.w = static_cast<float>(texture->w) * scale_x;
	rect.h = static_cast<float>(texture->h) * scale_y;

	if (!ignore_view_zoom)
	{
		rect.x -= view_position.x;
		rect.y -= view_position.y;

		zoomRect(rect);
	}

	SDL_RenderTexture(renderer, texture, nullptr, &rect);
}

void Renderer::zoomRect(SDL_FRect& rect) const
{
	glm::vec2 midscreen{ window.getWindowSize().x / 2.f, window.getWindowSize().y / 2.f };

	rect.x -= midscreen.x;
	rect.y -= midscreen.y;
	
	rect.x *= zoom;
	rect.y *= zoom;
	rect.w *= zoom;
	rect.h *= zoom;
	
	rect.x += midscreen.x;
	rect.y += midscreen.y;

	rect.w += 1.f;
	rect.h += 1.f;
	
	rect.x = std::floor(rect.x);
	rect.y = std::floor(rect.y);
}

void Renderer::setColor(Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

