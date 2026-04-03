#include <iostream>

#include "Text.hpp"

#include "Color.hpp"
#include "GpuRenderer.hpp"
#include "Surface.hpp"

namespace graphics
{

	Text::Text(GpuRenderer& renderer, std::shared_ptr<Font> font, const std::string& text, Color color, std::optional<int> wrapped_width) : text(text), font(font), color(color), wrapped_width(wrapped_width)
	{
		generateTextTexture(renderer);
	}

	std::shared_ptr<GpuTexture> Text::getTexture() const
	{
		return texture;
	}

	const std::string& Text::getText() const
	{
		return text;
	}

	glm::vec2 Text::getTextSize(const glm::vec2& scale) const
	{
		return {texture->w() * scale.x, texture->h() * scale.y};
	}

	void Text::setFont(std::shared_ptr<Font> font)
	{
		this->font = font;
		is_dirty = true;
	}


	void Text::setColor(Color color)
	{
		this->color = color;
		is_dirty = true;
	}

	void Text::setText(const std::string& text)
	{
		this->text = text;
		is_dirty = true;
	}

	void Text::setWrappedWidth(int wrapped_width)
	{
		this->wrapped_width = wrapped_width;
		is_dirty = true;
	}

	void Text::updateText(graphics::GpuRenderer& renderer)
	{
		if (is_dirty)
		{
			generateTextTexture(renderer);
			is_dirty = false;
		}
	}

	void Text::generateTextTexture(graphics::GpuRenderer& renderer)
	{
		Surface surface{ *font, text, color, wrapped_width };

		texture = renderer.loadTexture(surface);
	}
} // namespace graphics