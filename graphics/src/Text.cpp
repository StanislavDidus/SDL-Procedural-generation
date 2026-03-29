#include <iostream>

#include "Text.hpp"

#include "Color.hpp"
#include "GpuRenderer.hpp"
#include "Surface.hpp"

namespace graphics
{

	Text::Text(const Font* font, GpuRenderer& renderer, const std::string& text, Color color, std::optional<int> wrapped_width) : text(text), font(font), color(color), wrapped_width(wrapped_width)
	{
		//TTF_Text* ttf_text = TTF_CreateText(renderer.getTextEngine()->get(), font->getFont(), text.c_str(), text.length());
		
	}

	Text::~Text()
	{
	}

	const std::string& Text::getText() const
	{
		return text;
	}

	glm::vec2 Text::getTextSize(const glm::vec2& scale) const
	{
		return {};
	}

	void Text::setFont(const Font* font)
	{
		
	}


	void Text::setColor(Color color)
	{
	}

	void Text::setText(const std::string& text)
	{
	}

	void Text::setWrappedWidth(int wrapped_width)
	{
	}

	void Text::updateText(graphics::GpuRenderer& renderer)
	{
	}

	void Text::loadTexture(graphics::GpuRenderer& renderer, const Surface& surface)
	{
	}

	void Text::generateTextTexture(graphics::GpuRenderer& renderer)
	{
	}
} // namespace graphics