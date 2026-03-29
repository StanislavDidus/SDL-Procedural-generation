#pragma once


namespace graphics
{
	
	/*inline void setColor(Renderer& renderer, const Color& color)
	{
		SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
	}

	inline void setTextureColor(SDL_Texture* texture, const Color& color)
	{
		SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	}

	inline void clear(Renderer& renderer, const Color& color)
	{
		setColor(renderer, color);

		SDL_RenderClear(renderer.get());
	}

	inline void update(Renderer& renderer)
	{
		SDL_RenderPresent(renderer.get());
	}

	inline void drawPoint(Renderer& renderer, int x, int y, const Color& color)
	{
		drawPoint(renderer, static_cast<float>(x), static_cast<float>(y), color);
	}

	inline void drawPoint(Renderer& renderer, float x, float y, const Color& color)
	{
		const auto& view_position = renderer.getView();
		float zoom = renderer.getZoom();

		const glm::vec2 mid_screen{
			static_cast<float>(renderer.getWindowSize().x) / 2.f,
			static_cast<float>(renderer.getWindowSize().y) / 2.f
		};

		setColor(renderer, color);

		glm::vec2 p{ x, y };
		p -= view_position;
		zoomPoint(renderer, p.x, p.y);

		SDL_RenderPoint(renderer.get(), x, y);
	}

	inline void drawLine(Renderer& renderer, int x1, int y1, int x2, int y2, const Color& color)
	{
		drawLine(renderer, static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2), color);
	}

	inline void drawLine(Renderer& renderer, float x1, float y1, float x2, float y2, const Color& color)
	{
		const auto& view_position = renderer.getView();
		float zoom = renderer.getZoom();
		
		const glm::vec2 mid_screen{
			static_cast<float>(renderer.getWindowSize().x) / 2.f,
			static_cast<float>(renderer.getWindowSize().y) / 2.f
		};

		setColor(renderer, color);

		float x1_ = x1 - view_position.x - mid_screen.x;
		float y1_ = y1 - view_position.y - mid_screen.y;
		float x2_ = x2 - view_position.x - mid_screen.x;
		float y2_ = y2 - view_position.y - mid_screen.y;

		x1_ *= zoom;
		y1_ *= zoom;
		x2_ *= zoom;
		y2_ *= zoom;

		x1_ += mid_screen.x;
		y1_ += mid_screen.y;
		x2_ += mid_screen.x;
		y2_ += mid_screen.y;

		SDL_RenderLine(renderer.get(), x1_, y1_, x2_, y2_);
	}

	inline void drawCircle(Renderer& renderer, int centre_x, int centre_y, int radius, const Color& color, bool ignore_view_zoom = false)
	{
		drawCircle(renderer, static_cast<float>(centre_x), static_cast<float>(centre_y), static_cast<float>(radius), color, ignore_view_zoom);
	}

	// Code : https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
	inline void drawCircle(Renderer& renderer, float centre_x, float centre_y, float radius, const Color& color, bool ignore_view_zoom = false)
	{
		const float diameter = radius * 2.0f;
		float x = radius - 1.0f;
		float y = 0.0f;
		float tx = 1.0f;
		float ty = 1.0f;
		float error = (tx - diameter);

		while (x >= y)
		{
			drawPoint(renderer, centre_x + x, centre_y - y, color);
			drawPoint(renderer, centre_x + x, centre_y + y, color);
			drawPoint(renderer, centre_x - x, centre_y - y, color);
			drawPoint(renderer, centre_x - x, centre_y + y, color);
			drawPoint(renderer, centre_x + y, centre_y - x, color);
			drawPoint(renderer, centre_x + y, centre_y + x, color);
			drawPoint(renderer, centre_x - y, centre_y - x, color);
			drawPoint(renderer, centre_x - y, centre_y + x, color);

			if (error <= 0.0f)
			{
				y += 1.0f;
				error += ty;
				ty += 2.0f;
			}

			if (error > 0.0f)
			{
				x -= 1.0f;
				tx += 2.0f;
				error += (tx - diameter);
			}
		}
		
	}

	inline void drawRectangle(Renderer& renderer, int x, int y, int w, int h, RenderType render_type, const Color& color, bool ignore_view_zoom = false)
	{
		drawRectangle(renderer, static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h), render_type, color, ignore_view_zoom);
	}

	inline void drawRectangle(Renderer& renderer, float x, float y, float w, float h, RenderType render_type, const Color& color, bool ignore_view_zoom = false)
	{
		const auto& view_position = renderer.getView();

		setColor(renderer, color);

		SDL_FRect rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		if (!ignore_view_zoom)
		{
			rect.x -= view_position.x;
			rect.y -= view_position.y;

			zoomRect(renderer, rect);
		}

		switch (render_type)
		{
		case RenderType::NONE:
			SDL_RenderRect(renderer.get(), &rect);
			break;
		case RenderType::FILL:
			SDL_RenderFillRect(renderer.get(), &rect);
			break;
		}
	}

	inline void drawSprite(Renderer& renderer, const Sprite& sprite, int x, int y)
	{
		drawSprite(renderer, sprite, static_cast<float>(x), static_cast<float>(y));
	}

	inline void drawSprite(Renderer& renderer, const Sprite& sprite, float x, float y)
	{
		const auto& view_position = renderer.getView();

		SDL_FRect src, dst;

		src = sprite.getRect();

		dst.x = x - view_position.x;
		dst.y = y - view_position.y;
		dst.w = src.w;
		dst.h = src.h;

		zoomRect(renderer, dst);

	}

	inline void drawScaledSprite(Renderer& renderer, const Sprite& sprite, int x, int y, int width, int height, bool ignore_view_zoom = false)
	{
		drawScaledSprite(renderer, sprite, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
	}

	inline void drawScaledSprite(Renderer& renderer, const Sprite& sprite, float x, float y, float width, float height, bool ignore_view_zoom = false)
	{
		const auto& view_position = renderer.getView();

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

			zoomRect(renderer, dst);
		}

	}

	inline void drawRotatedSprite(Renderer& renderer, const Sprite& sprite, int x, int y, int width, int height, int angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE, bool ignore_view_zoom = false)
	{
		drawRotatedSprite(renderer, sprite, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), static_cast<float>(angle), flip_mode, ignore_view_zoom);
	}

	inline void drawRotatedSprite(Renderer& renderer, const Sprite& sprite, float x, float y, float width, float height, float angle, SDL_FlipMode flip_mode = SDL_FLIP_NONE, bool ignore_view_zoom = false)
	{
		const auto& view_position = renderer.getView();

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

			zoomRect(renderer, dst);
		}

	}*/

} //namespace graphics