#pragma once

#include <SDL3/SDL.h>
#include "Window.hpp"
#include "glm/glm.hpp"
#include "Color.hpp"

const float BaseWidthScreen = 960.f;
const float BaseHeightScreen = 540.f;

const float TileWidth = 25.f;
const float TileHeight = 25.f;

const float PlayerWidth = 25.f;
const float PlayerHeight = 50.f;

class Sprite;

enum class RenderType
{
	NONE,
	FILL
};

constexpr bool IGNORE_VIEW_ZOOM = true;

class Renderer
{
public:
	Renderer(Window& window);
	~Renderer();

	//Setters
	void setView(const glm::vec2& view_position);
	void setZoom(float zoom);
	
	//Getters
	const glm::vec2& getView() const;
	float getZoom() const;
	SDL_Renderer* getRenderer() const;
	const glm::ivec2& getWindowSize() const;

	void clear(Color color);

	void update(float dt);

	void drawPoint(int x, int y, Color color);
	void drawPoint(float x, float y, Color color);

	void drawLine(int x1, int y1, int x2, int y2, Color color);
	void drawLine(float x1, float y1, float x2, float y2, Color color);

	void drawRectangle(int x, int y, int w, int h, RenderType render_type, Color color, bool ignore_view_zoom = false);
	void drawRectangle(float x, float y, float w, float h, RenderType render_type, Color color, bool ignore_view_zoom = false);

	void drawSprite(const Sprite& sprite, int x, int y);
	void drawSprite(const Sprite& sprite, float x, float y);

	void drawScaledSprite(const Sprite& sprite, int x, int y, int width, int height, bool ignore_view_zoom = false);
	void drawScaledSprite(const Sprite& sprite, float x, float y, float width, float height, bool ignore_view_zoom = false);

	void drawRotatedSprite(const Sprite& sprite, int x, int y, int width, int height, int angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE);
	void drawRotatedSprite(const Sprite& sprite, float x, float y, float width, float height, float angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE);

	void drawUI(const Sprite& sprite, float x, float y, float width, float height, float angle = 0.f, SDL_FlipMode flip_mode = SDL_FLIP_NONE);
private:
	void zoomRect(SDL_FRect& rect) const;

	SDL_Renderer* renderer = nullptr;
	Window& window;

	glm::vec2 view_position{0.f, 0.f};
	float zoom = 1.5f;

	inline void setColor(Color color);
};