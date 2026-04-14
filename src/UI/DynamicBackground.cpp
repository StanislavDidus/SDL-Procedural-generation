#include "UI/DynamicBackground.hpp"

#include "ResourceManager.hpp"

using namespace graphics;

	DynamicBackground::DynamicBackground()
{
	const auto& spritesheet = ResourceManager::get().getSpriteSheet("backgrounds");
	const auto& spritesheet1 = ResourceManager::get().getSpriteSheet("cave_backgrounds");
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg2"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.55f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg3"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.25f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg4"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.1f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg5"));

	for (int i = 1; i < 7; ++i)
	{
		backgrounds.emplace_back(glm::vec2{ 0.0f, -540.0f * i }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg1"));
	}
	for (int i = 1; i < 8; ++i)
	{
		backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f * i }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg1"));
		backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f * i }, 0.50f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg2"));
		backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f * i }, 0.25f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg3"));
	}
}

void DynamicBackground::update(const glm::vec2& camera_position)
{
	for (auto& background : backgrounds)
	{
		glm::vec2 movement = camera_position * (1.0f - background.parallax_effect);

		if (movement.x > background.start_pos.x + background.size.x)
			background.start_pos.x += background.size.x;
		if (movement.x < background.start_pos.x - background.size.x)
			background.start_pos.x -= background.size.x;
	}
}

void DynamicBackground::render(GpuRenderer& screen, const glm::vec2& camera_position) const
{
	const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());
	for (const auto& background : backgrounds)
	{
		glm::vec2 distance = camera_position * background.parallax_effect;

		glm::vec2 new_position = background.start_pos + offset + distance;

		float visible_width = window_size.x / screen.getZoom();		
		int tiles = std::ceil(visible_width / background.size.x) + 2;
		int half_tiles = static_cast<int>(std::ceil(tiles / 2.0f));
		for (int i = -half_tiles; i <= half_tiles; ++i)
		{
			float x = new_position.x + i * background.size.x;

			glm::vec2 world_position{ x, background.start_pos.y + offset.y };
			drawScaledSprite(screen, background.sprite, world_position.x, world_position.y, background.size.x, background.size.y);
		}
	}
}
