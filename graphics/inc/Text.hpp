#pragma once

#include <string>
#include "Font.hpp"
#include "Color.hpp"
#include "GpuTexture.hpp"
#include "glm/vec2.hpp"

struct SDL_Texture;

namespace graphics
{
	class GpuRenderer;
	class Surface;

	class  Text
	{
	public:
		Text(graphics::GpuRenderer& renderer, std::shared_ptr<Font> font, const std::string& text, Color color = Color::BLACK, std::optional<int> wrapped_width = std::nullopt);
		~Text() = default;

		Text(const Text& other) = delete;
		Text(Text&& other) noexcept = delete;
		Text& operator=(const Text& other) = delete;
		Text& operator=(Text&& other) noexcept = delete;

		//Getters
		std::shared_ptr<GpuTexture> getTexture() const;
		const std::string& getText() const;
		glm::vec2 getTextSize(const glm::vec2& scale) const;

		//Setters
		void setFont(std::shared_ptr<Font> font);
		void setColor(Color color);
		void setText(const std::string& text);
		void setWrappedWidth(int wrapped_width);

		void updateText(graphics::GpuRenderer& renderer);
	private:
		void generateTextTexture(graphics::GpuRenderer& renderer);

		std::string text;
		std::shared_ptr<GpuTexture> texture = nullptr;
		std::shared_ptr<Font> font;
		Color color;
		std::optional<int> wrapped_width = std::nullopt;

		bool is_dirty = false;
	};

} // namespace graphics