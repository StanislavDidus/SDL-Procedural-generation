#pragma once

#include <memory>
#include <vector>

#include "RenderFunctions.hpp"
#include "Sprite.hpp"
#include "glm/glm.hpp"

namespace graphics
{
	class Renderer;
	class Surface;

	using Texture = std::shared_ptr<SDL_Texture>;
	
	using SpriteList = std::vector<std::pair<std::string, SDL_FRect>>;

	class SpriteSheet
	{
	public:
		SpriteSheet(graphics::Renderer& renderer, const Surface& surface, const SpriteList& sprite_list, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
		//SpriteSheet(graphics::Renderer& renderer, const Surface& surface, const glm::vec2& sprite_size, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);

		SpriteSheet(const SpriteSheet& other) = delete;
		SpriteSheet(SpriteSheet&& other) noexcept = delete;
		SpriteSheet& operator=(const SpriteSheet& other) = delete;
		SpriteSheet& operator=(SpriteSheet&& other) noexcept = delete;

		template<typename Self>
		auto&& getSprite(this Self&& self, const std::string& name);
		template<typename Self>
		auto&& getSprite(this Self&& self, size_t index);

		template<typename Self>
		auto&& operator[](this Self&& self, size_t index);
	private:
		Texture loadTexture(graphics::Renderer& renderer, const Surface& surface, SDL_ScaleMode scale_mode);

		std::vector<Sprite> sprites;
		std::unordered_map<std::string, size_t> nameToIndex;
	};

	template <typename Self>
	auto&& SpriteSheet::getSprite(this Self&& self, const std::string& name)
	{
		return self.sprites[self.nameToIndex[name]];
	}

	template <typename Self>
	auto&& SpriteSheet::getSprite(this Self&& self, size_t index)
	{
		return self.sprites[index];
	}

	template <typename Self>
	auto&& SpriteSheet::operator[](this Self&& self, size_t index)
	{
		return self.sprites[index];
	}
} // namespace graphics
