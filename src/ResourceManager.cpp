#include "ResourceManager.hpp"

#include "Surface.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;

void ResourceManager::loadXml(const std::filesystem::path& path, const Renderer& screen)
{
	XMLDocument doc;
	doc.LoadFile(path.string().c_str());

	const auto& asset_listing_node = doc.FirstChildElement("assetListing");
	const auto& sprite_listing_node = asset_listing_node->FirstChildElement("spriteListing");

	for (auto* sprite_node = sprite_listing_node->FirstChildElement("sprite"); sprite_node != nullptr; sprite_node = sprite_node->NextSiblingElement())
	{
		std::string asset_name = sprite_node->Attribute("id");

		const char* path = sprite_node->FirstChildElement("path")->GetText();

		const char* scale_mode_text = sprite_node->FirstChildElement("scaleMode")->GetText();
		SDL_ScaleMode scale_mode;

		if (strcmp(scale_mode_text, "NEAREST") == 0)
		{
			scale_mode = SDL_SCALEMODE_NEAREST;
		}
		else if (strcmp(scale_mode_text, "LINEAR") == 0)
		{
			scale_mode = SDL_SCALEMODE_LINEAR;
		}
		else if (strcmp(scale_mode_text, "PIXELART") == 0)
		{
			scale_mode = SDL_SCALEMODE_PIXELART;
		}
		else
		{
			scale_mode = SDL_SCALEMODE_INVALID;
		}

		//See if spritesheet consists of sprites of the same size or of different rects
		const auto& size_node = sprite_node->FirstChildElement("size");

		//All sprites are of the same size
		if (size_node)
		{
			glm::vec2 sprite_size;
			size_node->QueryFloatAttribute("x", &sprite_size.x);
			size_node->QueryFloatAttribute("y", &sprite_size.y);

			addSpriteSheet(asset_name, screen, path, sprite_size, scale_mode);
		}
		//Different rects
		else
		{
			const auto& rect_listing_node = sprite_node->FirstChildElement("rectListing");

			std::vector<SDL_FRect> rects;
			for (auto* rect_node = rect_listing_node->FirstChildElement("rect"); rect_node != nullptr; rect_node = rect_node->NextSiblingElement())
			{
				SDL_FRect rect;
				rect_node->QueryFloatAttribute("x", &rect.x);
				rect_node->QueryFloatAttribute("y", &rect.y);
				rect_node->QueryFloatAttribute("w", &rect.w);
				rect_node->QueryFloatAttribute("h", &rect.h);
				rects.push_back(rect);
			}

			addSpriteSheet(asset_name, screen, path, rects, scale_mode);
		}

	}
}

const SpriteSheet& ResourceManager::getSpriteSheet(const std::string& name) const
{
	return *spritesheets.at(name);
}

void ResourceManager::addSpriteSheet(const std::string& name, const Renderer& screen, const std::filesystem::path& path, const glm::vec2& size,
                                            SDL_ScaleMode scale_mode)
{
	spritesheets[name] = std::make_unique<SpriteSheet>(screen, Surface{ path }, size, scale_mode);
	//spritesheets[name] = std::move(SpriteSheet{ screen, Surface{path}, size, scale_mode });
}

void ResourceManager::addSpriteSheet(const std::string& name, const Renderer& screen, const std::filesystem::path& path,
	const std::vector<SDL_FRect>& rects, SDL_ScaleMode scale_mode)
{
	spritesheets[name] = std::make_unique<SpriteSheet>(screen, Surface{ path }, rects, scale_mode);
	//spritesheets[name] = std::move(SpriteSheet{ screen, Surface{path}, rects, scale_mode });
}

