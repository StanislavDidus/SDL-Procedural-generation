#pragma once
#include "Color.hpp"
#include "GpuRenderer.hpp"
#include "Sprite.hpp"
#include "Text.hpp"

namespace graphics
{
	inline glm::vec2 getMouseGlobalPosition(const GpuRenderer& renderer, const glm::vec2& mouse_position)
	{
		const auto& view_position = renderer.getView();
		const auto& zoom = renderer.getZoom();
		const auto& window_size = renderer.getWindowSize();

		glm::vec2 mid_screen = { window_size.x / 2.f, window_size.y / 2.f };
		glm::vec2 view_centered = view_position + mid_screen;
		return view_centered + (mouse_position - mid_screen) / zoom;
	}
		
	inline SDL_FRect getCameraRectFromTarget(const GpuRenderer& renderer, const glm::vec2 target) noexcept
	{
		const auto& window_size = renderer.getWindowSize();
		const auto& view_position = renderer.getView();
		auto zoom = renderer.getZoom();

		float halfW = (window_size.x * 0.5f) / zoom;
		float halfH = (window_size.y * 0.5f) / zoom;

		//
		//glm::vec2 mid_view_position = view_position + static_cast<glm::vec2>(window_size) * 0.5f;
		//

		float left_world = target.x - halfW;
		float right_world = target.x + halfW;
		int begin_x = static_cast<int>(std::floor(left_world / 20.f));
		int end_x = static_cast<int>(std::ceil(right_world / 20.f));

		float up_world = target.y - halfH;
		float down_world = target.y + halfH;
		int begin_y = static_cast<int>(std::floor(up_world / 20.f));
		int end_y = static_cast<int>(std::ceil(down_world / 20.f));

		SDL_FRect camera_rect;
		camera_rect.x = left_world;
		camera_rect.y = up_world;
		camera_rect.w = right_world - left_world;
		camera_rect.h = down_world - up_world;
		return camera_rect;
	}

	inline void zoomPoint(const GpuRenderer& renderer, float& x, float& y) noexcept
	{
		const glm::vec2 mid_screen{
			static_cast<float>(renderer.getWindowSize().x) / 2.f,
			static_cast<float>(renderer.getWindowSize().y) / 2.f
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
			static_cast<float>(renderer.getWindowSize().x) / 2.f,
			static_cast<float>(renderer.getWindowSize().y) / 2.f
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
		renderer.renderRectangle(x, y, w, h, type, color, ignore_view_zoom);
	}

	inline void drawScaledSprite(GpuRenderer& renderer, const Sprite& sprite, float x, float y, float w, float h, bool ignore_view_zoom = false)
	{
		renderer.renderSprite(sprite, x, y, w, h, 0.0f, SDL_FLIP_NONE, ignore_view_zoom);
	}

	inline void drawRotatedSprite(GpuRenderer& renderer, const Sprite& sprite, float x, float y, float w, float h, float angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE, bool ignore_view_zoom = false)
	{
		renderer.renderSprite(sprite, x, y, w, h, angle, flip_mode, ignore_view_zoom);
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
