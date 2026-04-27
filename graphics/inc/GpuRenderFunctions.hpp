#pragma once
#include "Color.hpp"
#include "GpuRenderer.hpp"
#include "Sprite.hpp"
#include "Text.hpp"

namespace graphics
{
	inline glm::vec2 getMouseGlobalPosition(const GpuRenderer& renderer, const glm::vec2& mouse_position)
	{
		const auto& view = renderer.getView();
		const auto& zoom = renderer.getZoom();
		const auto& window_size = renderer.getStandardWindowSize();

		glm::vec2 mid_screen {window_size.x * 0.5f, window_size.y * 0.5f};

		glm::vec2 result{ view + mid_screen + (mouse_position - mid_screen) / zoom };
		return result;
	}
		
	inline SDL_FRect getCameraRect(const GpuRenderer& renderer) noexcept
	{
		const auto& window_size = renderer.getStandardWindowSize();
		const auto& view = renderer.getView();
		auto zoom = renderer.getZoom();
		
		float world_width = window_size.x / zoom;
		float world_height = window_size.y / zoom;
		
		float offset_x = (window_size.x - world_width) * 0.5f;
		float offset_y = (window_size.y - world_height) * 0.5f;

		return SDL_FRect{
        view.x + offset_x,
        view.y + offset_y,
        world_width	,
        world_height};
	}

	inline void zoomPoint(const GpuRenderer& renderer, float& x, float& y) noexcept
	{
		const glm::vec2 mid_screen{
			static_cast<float>(renderer.getStandardWindowSize().x) / 2.f,
			static_cast<float>(renderer.getStandardWindowSize().y) / 2.f
		};

		const float zoom = renderer.getZoom();

		x -= mid_screen.x;
		y -= mid_screen.y;

		x *= zoom;
		y *= zoom;

		x += mid_screen.x;
		y += mid_screen.y;

		x = std::floor(x);
		y = std::floor(y);
	}

	inline void zoomRect(const GpuRenderer& renderer, SDL_FRect& rect) noexcept
	{
		const glm::vec2 mid_screen{
			static_cast<float>(renderer.getStandardWindowSize().x) / 2.f,
			static_cast<float>(renderer.getStandardWindowSize().y) / 2.f
		};
		const float zoom = renderer.getZoom();

		rect.x -= mid_screen.x;
		rect.y -= mid_screen.y;

		rect.x *= zoom;
		rect.y *= zoom;
		rect.w *= zoom;
		rect.h *= zoom;

		rect.x += mid_screen.x;
		rect.y += mid_screen.y;

		rect.w += 1.f;
		rect.h += 1.f;

		rect.x = std::floor(rect.x);
		rect.y = std::floor(rect.y);
	}

	inline void drawRectangle(GpuRenderer& renderer, float x, float y, float w, float h, RenderType type, const Color& color, bool ignore_view_zoom = false)
	{
		glm::vec4 fcolor{ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
		renderer.renderRectangle(x, y, w, h, type, fcolor, ignore_view_zoom);
	}

	inline void drawScaledSprite(GpuRenderer& renderer, const Sprite& sprite, float x, float y, float w, float h, bool ignore_view_zoom = false)
	{
		renderer.renderSprite(sprite, x, y, w, h, 0.0f, SDL_FLIP_NONE, ignore_view_zoom);
	}

	inline void drawRotatedSprite(GpuRenderer& renderer, const Sprite& sprite, float x, float y, float w, float h, float angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE, bool ignore_view_zoom = false, graphics::Color color = graphics::Color::WHITE)
	{
		renderer.renderSprite(sprite, x, y, w, h, angle, flip_mode, ignore_view_zoom, color);
	}

	inline void printText(GpuRenderer& renderer, Text& text, float x, float y, float w, float h, bool ignore_view_zoom = false)
	{
		renderer.renderTexture(text.getTexture(), std::nullopt, SDL_FRect{ x,y,w,h }, 0.0f, SDL_FLIP_NONE, ignore_view_zoom);
	}

	inline void printTextScaled(GpuRenderer& renderer, Text& text, float x, float y, float scale_x, float scale_y, bool ignore_view_zoom = false)
	{
		glm::vec2 texture_size = {text.getTexture()->w(), text.getTexture()->h()};
		renderer.renderTexture(text.getTexture(), std::nullopt, SDL_FRect{ x,y,texture_size.x * scale_x, texture_size.y * scale_y }, 0.0f, SDL_FLIP_NONE, ignore_view_zoom);
	}
}
