#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

#include "AudioDevice.hpp"
#include <graphics/SpriteSheet.hpp>
#include <graphics/Font.hpp>
#include "Sound.hpp"

class ResourceManager
{
public:
	static ResourceManager& get()
	{
		static ResourceManager s;
		return s;
	}

	void loadXml(const std::filesystem::path& path_, graphics::Renderer& screen);

	std::shared_ptr<graphics::Font> getFont(const std::string& name) const;
	std::shared_ptr<graphics::SpriteSheet> getSpriteSheet(const std::string& name) const;
	std::shared_ptr<audio::Sound> getSound(const std::string& name) const;
	
	void setMasterVolume(float volume);
	
	void updateSounds();
private:
	ResourceManager();

	void addSpriteSheet(const std::string& name, graphics::Renderer& screen, const std::filesystem::path& path, const graphics::SpriteList& sprite_list, graphics
	                    ::TextureScaleMode scale_mode = graphics::TextureScaleMode::LINEAR);
	void addFont(const std::string& name, const std::filesystem::path& path, int size);
	void addSound(const std::string& name, const std::filesystem::path& path, float volume, audio::Sound::PlayType type, bool loop);

	std::unordered_map<std::string, std::shared_ptr<graphics::SpriteSheet>> spritesheets;
	std::unordered_map<std::string, std::shared_ptr<graphics::Font>> fonts;
	std::unordered_map<std::string, std::shared_ptr<audio::Sound>> sounds;
	std::vector<std::shared_ptr<audio::Sound>> looped_sounds;
	
	audio::AudioDevice audio_device;
	float master_volume = 1.0f;
};
