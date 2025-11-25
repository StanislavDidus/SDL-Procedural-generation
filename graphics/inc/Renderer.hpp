#pragma once

#include <SDL3/SDL.h>
#include "Window.hpp"
#include "glm/glm.hpp"

class Sprite;

struct Color
{
	Color() = default;
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
	Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
	Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
	virtual ~Color() {}

	Color operator+ (const Color& opperand) const
	{ 
		return { static_cast<uint8_t>(r + opperand.r),
				static_cast<uint8_t>(g + opperand.g),
				static_cast<uint8_t>(b + opperand.b),
				static_cast<uint8_t>(a + opperand.a) };
	}
	Color operator- (const Color& opperand) const
	{
		return { static_cast<uint8_t>(r - opperand.r),
				static_cast<uint8_t>(g - opperand.g),
				static_cast<uint8_t>(b - opperand.b),
				static_cast<uint8_t>(a - opperand.a)
		};
	}
	
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	static const Color BLACK;
	static const Color GREY;
	static const Color WHITE;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
};

enum class RenderType
{
	NONE,
	FILL
};


class Renderer
{
public:
	Renderer(Window& window);
	virtual ~Renderer();

	//Setters
	void setView(const glm::vec2& view_position);

	//Getters
	const glm::vec2& getView() const;
	SDL_Renderer* getRenderer() const;
	const glm::ivec2& getWindowSize() const;

	void clear(Color color);

	void update(float dt);

	void drawPoint(int x, int y, Color color);
	void drawPoint(float x, float y, Color color);

	void drawLine(int x1, int y1, int x2, int y2, Color color);
	void drawLine(float x1, float y1, float x2, float y2, Color color);

	void drawRectangle(int x, int y, int w, int h, RenderType render_type, Color color);
	void drawRectangle(float x, float y, float w, float h, RenderType render_type, Color color);

	void drawSprite(const Sprite& sprite, int x, int y);
	void drawSprite(const Sprite& sprite, float x, float y);

	void drawScaledSprite(const Sprite& sprite, int x, int y, int width, int height);
	void drawScaledSprite(const Sprite& sprite, float x, float y, float width, float height);

	void drawRotatedSprite(const Sprite& sprite, int x, int y, int width, int height, int angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE);
	void drawRotatedSprite(const Sprite& sprite, float x, float y, float width, float height, float angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE);

	void drawUI(const Sprite& sprite, float x, float y, float width, float height, float angle = 0.f, SDL_FlipMode flip_mode = SDL_FLIP_NONE);
private:
	SDL_Renderer* renderer = nullptr;
	Window& window;

	glm::vec2 view_position{0.f, 0.f};

	inline void setColor(Color color);
};