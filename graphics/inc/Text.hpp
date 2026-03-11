#pragma once

#include <string>
#include "Font.hpp"
#include "Color.hpp"
#include "glm/vec2.hpp"

struct SDL_Texture;

namespace graphics
{
	class Renderer;
	class Surface;

	class  Text
	{
	public:
		Text(const Font* font, graphics::Renderer& renderer, const std::string& text, Color color = Color::BLACK, std::optional<int> wrapped_width = std::nullopt);
		~Text();

		Text(const Text& other) = delete;
		Text(Text&& other) noexcept = delete;
		Text& operator=(const Text& other) = delete;
		Text& operator=(Text&& other) noexcept = delete;

		//Getters
		SDL_Texture* getTexture() const;
		const std::string& getText() const;
		glm::vec2 getTextSize(const glm::vec2& scale) const;

		//Setters
		void setFont(const Font* font);
		void setColor(Color color);
		void setText(const std::string& text);
		void setWrappedWidth(int wrapped_width);

		void updateText(graphics::Renderer& renderer);
	private:
		void loadTexture(graphics::Renderer& renderer, const Surface& surface);
		void generateTextTexture(graphics::Renderer& renderer);

		std::string text;
		SDL_Texture* texture;
		const Font* font;
		Color color;
		std::optional<int> wrapped_width;

		bool is_dirty = false;
	};

} // namespace graphics