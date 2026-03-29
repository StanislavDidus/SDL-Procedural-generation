#pragma once

#include <memory>
#include <vector>

#include "GpuRenderer.hpp"
#include "GpuRenderFunctions.hpp"
#include "Sprite.hpp"
#include "glm/glm.hpp"

namespace graphics
{
	using SpriteList = std::vector<std::pair<std::string, SDL_FRect>>;

	class SpriteSheet
	{
	public:
		SpriteSheet(graphics::GpuRenderer& renderer, const std::filesystem::path& path, const SpriteList& sprite_list, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
		//SpriteSheet(graphics::GpuRenderer& renderer, const Surface& surface, const glm::vec2& sprite_size, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);

		SpriteSheet(const SpriteSheet& other) = delete;
		SpriteSheet(SpriteSheet&& other) noexcept = delete;
		SpriteSheet& operator=(const SpriteSheet& other) = delete;
		SpriteSheet& operator=(SpriteSheet&& other) noexcept = delete;

		std::shared_ptr<GpuTexture> getTexture() const;

		template<typename Self>
		auto&& getSprite(this Self&& self, const std::string& name);
		template<typename Self>
		auto&& getSprite(this Self&& self, size_t index);

		template<typename Self>
		auto&& operator[](this Self&& self, size_t index);

	private:
		std::shared_ptr<GpuTexture> texture = nullptr;

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
