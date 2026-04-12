#include "UI/DynamicBackground.hpp"

#include "ResourceManager.hpp"

using namespace graphics;

DynamicBackground::DynamicBackground()
{
	const auto& spritesheet = ResourceManager::get().getSpriteSheet("backgrounds");
	const auto& spritesheet1 = ResourceManager::get().getSpriteSheet("cave_backgrounds");
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 1.0f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg1"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.9f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg2"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.6f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg3"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.3f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg4"));
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.1f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg5"));

	cave_backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 1.0f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg1"));
	cave_backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.8f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg2"));
	cave_backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.6f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg3"));
}

void DynamicBackground::update(const glm::vec2& camera_position)
{
	for (int i = 0; auto& background : backgrounds)
	{
		float movement = camera_position.x * (1.0f - background.parallax_effect);
		if (movement > background.start_pos.x + background.size.x)
		{
			background.start_pos.x += background.size.x;
		}
		else if (movement < background.start_pos.x - background.size.x)
		{
			background.start_pos.x -= background.size.x;
		}

		if (i == 0)
		{
			float movement_y = camera_position.y;
			if (movement_y > start_pos_y + background.size.y)
			{
				start_pos_y += background.size.y;
			}
			else if (movement_y < start_pos_y - background.size.y)
			{
				start_pos_y -= background.size.y;
			}
		}
			
		++i;

		background.start_pos.y = std::min(background.start_pos.y, 0.0f);
		start_pos_y = std::min(start_pos_y, 0.0f);
	}

	for (auto& background : cave_backgrounds)
	{
		glm::vec2 movement = camera_position * (1.0f - background.parallax_effect);
		if (movement.x > background.start_pos.x + background.size.x)
		{
			background.start_pos.x += background.size.x;
		}
		else if (movement.x < background.start_pos.x - background.size.x)
		{
			background.start_pos.x -= background.size.x;
		}

		if (movement.y > background.start_pos.y + background.size.y)
		{
			background.start_pos.y += background.size.y;
		}
		else if (movement.y < background.start_pos.y - background.size.y)
		{
			background.start_pos.y -= background.size.y;
		}
	}
}

void DynamicBackground::render(GpuRenderer& screen, const glm::vec2& camera_position) const
{
	const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());
	for (int i = 0; const auto& background : backgrounds)
	{
		glm::vec2 new_size = background.size / screen.getZoom();

		float distance = camera_position.x * background.parallax_effect;

		glm::vec2 new_position = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + offset.x, background.start_pos.y + offset.y });
		glm::vec2 new_position2 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance - background.size.x + offset.x, background.start_pos.y + offset.y });
		glm::vec2 new_position3 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + background.size.x + offset.x, background.start_pos.y + offset.y });
		drawScaledSprite(screen, background.sprite, new_position.x, new_position.y, new_size.x, new_size.y);
		drawScaledSprite(screen, background.sprite, new_position2.x, new_position2.y, new_size.x, new_size.y);
		drawScaledSprite(screen, background.sprite, new_position3.x, new_position3.y, new_size.x, new_size.y);

		if (i == 0)
		{
			glm::vec2 new_position_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + offset.x, start_pos_y + offset.y });
			glm::vec2 new_position2_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance - background.size.x + offset.x, start_pos_y + offset.y });
			glm::vec2 new_position3_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + background.size.x + offset.x, start_pos_y + offset.y });

			glm::vec2 new_position4_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + offset.x, start_pos_y + offset.y + background.size.y });
			glm::vec2 new_position5_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance - background.size.x + offset.x, start_pos_y + offset.y + background.size.y });
			glm::vec2 new_position6_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + background.size.x + offset.x, start_pos_y + offset.y + background.size.y });

			glm::vec2 new_position7_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + offset.x, start_pos_y + offset.y - background.size.y });
			glm::vec2 new_position8_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance - background.size.x + offset.x, start_pos_y + offset.y - background.size.y });
			glm::vec2 new_position9_ = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance + background.size.x + offset.x, start_pos_y + offset.y - background.size.y });

			drawScaledSprite(screen, background.sprite, new_position_.x, new_position_.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position2_.x, new_position2_.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position3_.x, new_position3_.y, new_size.x, new_size.y);

			drawScaledSprite(screen, background.sprite, new_position4_.x, new_position4_.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position5_.x, new_position5_.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position6_.x, new_position6_.y, new_size.x, new_size.y);

			drawScaledSprite(screen, background.sprite, new_position7_.x, new_position7_.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position8_.x, new_position8_.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position9_.x, new_position9_.y, new_size.x, new_size.y);
		}

		++i;
	}

	for (const auto& background : cave_backgrounds)
	{
		glm::vec2 distance = camera_position * background.parallax_effect;
		glm::vec2 new_size = background.size / screen.getZoom();

		for (int i = -1; i <= 1; ++i)
		{
			for (int j = -1; j <= 1; ++j)
			{
				float x = background.start_pos.x + distance.x + offset.x + i * background.size.x;
				float y = background.start_pos.y + distance.y + offset.y + j * background.size.y;
				y = std::max(y, background.size.y);
				glm::vec2 new_position = getZoomedCoordinates(screen, glm::vec2{ x, y });
				drawScaledSprite(screen, background.sprite, new_position.x, new_position.y, new_size.x, new_size.y);
			}
		}
	}
}

glm::vec2 DynamicBackground::getZoomedCoordinates(const GpuRenderer& screen, const glm::vec2& position)
{
	const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());
	const auto& zoom = screen.getZoom();
	glm::vec2 mid_screen = glm::vec2{window_size * 0.5f} + screen.getView();
	return (position - mid_screen) / zoom + mid_screen;
}
