#pragma once

#include <string>
#include "Font.hpp"
#include "Color.hpp"

struct SDL_Texture;

namespace graphics
{
	class Renderer;
	class Surface;

	class  Text
	{
	public:
		Text(const Font* font, graphics::Renderer& renderer, const std::string& text, Color color = Color::BLACK);
		~Text();

		Text(const Text& other) = delete;
		Text(Text&& other) noexcept = delete;
		Text& operator=(const Text& other) = delete;
		Text& operator=(Text&& other) noexcept = delete;

		//Getters
		SDL_Texture* getTexture() const;
		const std::string& getText() const;

		//Setters
		void setFont(const Font* font);
		void setColor(Color color);
		void setText(const std::string& text);

		void updateText(graphics::Renderer& renderer);
	private:
		void loadTexture(graphics::Renderer& renderer, const Surface& surface);
		void generateTextTexture(graphics::Renderer& renderer);

		std::string text;
		SDL_Texture* texture;
		const Font* font;
		Color color;

		bool is_dirty = false;
	};

} // namespace graphics