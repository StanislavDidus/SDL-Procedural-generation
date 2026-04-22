#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

#include "AudioDevice.hpp"
#include "SpriteSheet.hpp"
#include "Font.hpp"
#include "Sound.hpp"

class ResourceManager
{
public:
	static ResourceManager& get()
	{
		static ResourceManager s;
		return s;
	}

	void loadXml(const std::filesystem::path& path_, graphics::GpuRenderer& screen);

	std::shared_ptr<graphics::Font> getFont(const std::string& name) const;
	std::shared_ptr<graphics::SpriteSheet> getSpriteSheet(const std::string& name) const;
	std::shared_ptr<audio::Sound> getSound(const std::string& name) const;
	
	void setMasterVolume(float volume);
private:
	ResourceManager();

	void addSpriteSheet(const std::string& name, graphics::GpuRenderer& screen, const std::filesystem::path& path, const graphics::SpriteList& sprite_list, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
	void addFont(const std::string& name, const std::filesystem::path& path, int size);
	void addSound(const std::string& name, const std::filesystem::path& path, float volume, audio::Sound::PlayType type);

	std::unordered_map<std::string, std::shared_ptr<graphics::SpriteSheet>> spritesheets;
	std::unordered_map<std::string, std::shared_ptr<graphics::Font>> fonts;
	std::unordered_map<std::string, std::shared_ptr<audio::Sound>> sounds;
	
	audio::AudioDevice audio_device;
	float master_volume = 1.0f;
};
