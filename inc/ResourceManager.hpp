#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

#include "SpriteSheet.hpp"

// TODO: Use smart pointers to store SpriteSheets in dynamic arrays or maps.

class ResourceManager
{
public:
	static ResourceManager& get()
	{
		static ResourceManager s;
		return s;
	}

	void loadXml(const std::filesystem::path& path, const Renderer& screen);

	const SpriteSheet& getSpriteSheet(const std::string& name) const;

	void addSpriteSheet(const std::string& name, const Renderer& screen, const std::filesystem::path& path, const glm::vec2& size, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
	void addSpriteSheet(const std::string& name, const Renderer& screen, const std::filesystem::path& path, const std::vector<SDL_FRect>& rects, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);

private:
	ResourceManager() = default;

	std::unordered_map<std::string, std::unique_ptr<SpriteSheet>> spritesheets;
	//std::unordered_map<std::string, size_t> nameToIndex; ///< Gives user an index to the place where the spritesheet with the given name lies in a vector
	//size_t spritesheets_counter = 0;
};