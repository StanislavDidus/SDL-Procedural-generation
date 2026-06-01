#include "UI/DynamicBackground.hpp"

#include "ResourceManager.hpp"
#include "GpuRenderFunctions.hpp"

using namespace graphics;

	DynamicBackground::DynamicBackground()
{
	const auto& spritesheet = ResourceManager::get().getSpriteSheet("backgrounds");
	const auto& spritesheet1 = ResourceManager::get().getSpriteSheet("cave_backgrounds");
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg1"));
		backgrounds.back().night = true;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg2"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.55f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg3"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.25f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg4"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.1f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg5"));

	for (int i = 1; i < 7; ++i)
	{
		backgrounds.emplace_back(glm::vec2{ 0.0f, -540.0f * i }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg1"));
		backgrounds.back().night = true;
	}
	for (int i = 1; i < 13; ++i)
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

void DynamicBackground::render(Renderer& screen, const glm::vec2& camera_position) const
{
	const auto& window_size = static_cast<glm::vec2>(screen.getStandardWindowSize());
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
			
			if (!background.night)
				drawScaledSprite(screen, background.sprite, world_position.x, world_position.y, background.size.x, background.size.y);
			else
			{
				uint8_t day_transparency = static_cast<uint8_t>((1.0f - global_time) * 255.0f);
				uint8_t night_transparency = static_cast<uint8_t>((global_time) * 255.0f);
				drawScaledSprite(screen, background.sprite, world_position.x, world_position.y, background.size.x, background.size.y, false, graphics::Color{255,255,255,day_transparency});
				drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("night_sky"), world_position.x, world_position.y, background.size.x, background.size.y, false, graphics::Color{255,255,255,night_transparency});
			}
		}
	}
}

void DynamicBackground::setGlobalTime(float time)
{
		global_time = time;
}
