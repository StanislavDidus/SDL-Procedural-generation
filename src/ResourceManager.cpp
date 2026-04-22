#include "ResourceManager.hpp"

#include <UI/Button.hpp>


#include "Surface.hpp"
#include "tinyxml2.h"

using namespace graphics;
using namespace tinyxml2;

void ResourceManager::loadXml(const std::filesystem::path& path_, graphics::GpuRenderer& screen)
{
	XMLDocument doc;
	doc.LoadFile(path_.string().c_str());

	const auto& asset_listing_node = doc.FirstChildElement("assetListing");
	const auto& sprite_listing_node = asset_listing_node->FirstChildElement("spriteListing");

	//Loop through sprites
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

		const auto& rect_listing_node = sprite_node->FirstChildElement("rectListing");

		SpriteList sprite_list;
		for (auto* rect_node = rect_listing_node->FirstChildElement("rect"); rect_node != nullptr; rect_node = rect_node->NextSiblingElement())
		{
			SDL_FRect rect;
			rect_node->QueryFloatAttribute("x", &rect.x);
			rect_node->QueryFloatAttribute("y", &rect.y);
			rect_node->QueryFloatAttribute("w", &rect.w);
			rect_node->QueryFloatAttribute("h", &rect.h);
			const char* name = rect_node->Attribute("name");
			sprite_list.emplace_back(name, rect);
		}

		addSpriteSheet(asset_name, screen, path, sprite_list, scale_mode);

	}

	//Loop through fonts
	const auto& font_listing_node = asset_listing_node->FirstChildElement("fontListing");
	for (auto* font_node = font_listing_node->FirstChildElement("font"); font_node != nullptr; font_node = font_node->NextSiblingElement())
	{
		const char* font_name = font_node->Attribute("id");
		const char* path = font_node->FirstChildElement("path")->GetText();
		int size;
		font_node->FirstChildElement("size")->QueryIntText(&size);

		addFont(font_name, path, size);
	}
	
	//Loop through sounds
	const auto& sound_listing_node = asset_listing_node->FirstChildElement("soundListing");
	for (auto* sound_node = sound_listing_node->FirstChildElement("sound"); sound_node != nullptr; sound_node = sound_node->NextSiblingElement())
	{
		const char* sound_name = sound_node->Attribute("id");
		const char* path = sound_node->FirstChildElement("path")->GetText();
		
		float volume = 1.0f;
		if (sound_node->FirstChildElement("volume") !=  nullptr)
			volume = sound_node->FirstChildElement("volume")->FloatText();
		
		audio::Sound::PlayType type = audio::Sound::PlayType::STOP_ON_PLAY;
		if (sound_node->FirstChildElement("type") != nullptr)
		{
			const char* type_text = sound_node->FirstChildElement("type")->GetText();
			if (strcmp(type_text, "DONT_PLAY_WHEN_PLAYING") == 0)
			{
				type = audio::Sound::PlayType::DONT_PLAY_WHEN_PLAYING;
			}
		}
		addSound(sound_name, path, volume, type);
	}
}

std::shared_ptr<Font> ResourceManager::getFont(const std::string& name) const
{
	return fonts.at(name);
}

std::shared_ptr<SpriteSheet> ResourceManager::getSpriteSheet(const std::string& name) const
{
	return spritesheets.at(name);
}

std::shared_ptr<audio::Sound> ResourceManager::getSound(const std::string& name) const
{
	return sounds.at(name);
}

void ResourceManager::setMasterVolume(float volume)
{
	for (auto& [name, sound] : sounds)
	{
		sound->setVolume(sound->getVolume() * volume);
	}
	master_volume = volume;
}

ResourceManager::ResourceManager()
	: audio_device{}
{
	
}

void ResourceManager::addSpriteSheet(const std::string& name, graphics::GpuRenderer& screen, const std::filesystem::path& path,
                                     const SpriteList& sprite_list, SDL_ScaleMode scale_mode)
{
	spritesheets[name] = std::make_shared<SpriteSheet>(screen, path, sprite_list, scale_mode);
}

void ResourceManager::addFont(const std::string& name, const std::filesystem::path& path, int size)
{
	fonts[name] = std::make_shared<Font>(path, size);
}

void ResourceManager::addSound(const std::string& name, const std::filesystem::path& path, float volume, audio::Sound::PlayType type)
{
	auto sound = std::make_shared<audio::Sound>(audio_device.getAudioDeviceID(), path);
	sound->setVolume(volume * master_volume);
	sound->setPlayType(type);
	sounds[name] = sound;
}

