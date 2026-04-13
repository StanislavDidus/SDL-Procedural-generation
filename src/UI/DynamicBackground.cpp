#include "UI/DynamicBackground.hpp"

#include "ResourceManager.hpp"

using namespace graphics;

DynamicBackground::DynamicBackground()
{
	const auto& spritesheet = ResourceManager::get().getSpriteSheet("backgrounds");
	const auto& spritesheet1 = ResourceManager::get().getSpriteSheet("cave_backgrounds");
	//backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 1.0f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg1"),BackgroundMoveType::BOTH, BackgroundRenderType::ABOVE);
	//backgrounds.back().min_x = 0.0f;
	//backgrounds.back().max_height = 0.0f;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg2"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
	backgrounds.back().min_x = 0.0f;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.55f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg3"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
	backgrounds.back().min_x = 0.0f;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.25f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg4"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
	backgrounds.back().min_x = 0.0f;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.1f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg5"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
	backgrounds.back().min_x = 0.0f;

	for (int i = 1; i < 7; ++i)
	{
		backgrounds.emplace_back(glm::vec2{ 0.0f, -540.0f * i }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet->getSprite("bg1"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
	}
	for (int i = 1; i < 8; ++i)
	{
		backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f * i }, 0.80f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg1"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
		backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f * i }, 0.50f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg2"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
		backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f * i }, 0.25f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg3"), BackgroundMoveType::HORIZONTAL, BackgroundRenderType::HORIZONTAL);
	}

	/*
	backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f }, 0.6f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg1"), BackgroundMoveType::BOTH, BackgroundRenderType::BOTH);
	backgrounds.back().min_height = 540.0f * 2.0f;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f }, 0.3f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg2"), BackgroundMoveType::BOTH, BackgroundRenderType::BOTH);
	backgrounds.back().min_height = 540.0f * 2.0f;
	backgrounds.emplace_back(glm::vec2{ 0.0f, 540.0f }, 0.1f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg3"), BackgroundMoveType::BOTH, BackgroundRenderType::BOTH);
	backgrounds.back().min_height = 540.0f * 2.0f;
	*/
	/*
	cave_backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 1.0f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg1"));
	cave_backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.8f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg2"));
	cave_backgrounds.emplace_back(glm::vec2{ 0.0f, 0.0f }, 0.6f, glm::vec2{ 960.0f,540.0f }, spritesheet1->getSprite("bg3"));
*/
}

void DynamicBackground::update(const glm::vec2& camera_position)
{
	for (auto& background : backgrounds)
	{
		glm::vec2 movement = camera_position * (1.0f - background.parallax_effect);
		if (background.move_type == BackgroundMoveType::HORIZONTAL || background.move_type == BackgroundMoveType::BOTH)
		{
			if (movement.x > background.start_pos.x + background.size.x)
				background.start_pos.x += background.size.x;
			if (movement.x < background.start_pos.x - background.size.x)
				background.start_pos.x -= background.size.x;
		}
		if (background.move_type == BackgroundMoveType::VERTICAL || background.move_type == BackgroundMoveType::BOTH)
		{
			if (movement.y > background.start_pos.y + background.size.y)
				background.start_pos.y += background.size.y;
			if (movement.y < background.start_pos.y - background.size.y)
				background.start_pos.y -= background.size.y;
		}

		/*if (background.max_height)
			background.start_pos.y = std::min(background.max_height.value(), background.start_pos.y);
		if (background.min_height)
			background.start_pos.y = std::max(background.min_height.value(), background.start_pos.y);*/
	}
}

void DynamicBackground::render(GpuRenderer& screen, const glm::vec2& camera_position) const
{
	const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());
	for (const auto& background : backgrounds)
	{
		glm::vec2 new_size = background.size;

		glm::vec2 distance = camera_position * background.parallax_effect;

		//glm::vec2 new_position {background.start_pos.x + distance.x, background.start_pos.y + offset.y};
		//glm::vec2 new_position = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + offset.x, background.start_pos.y + offset.y });
		glm::vec2 new_position = background.start_pos + offset;
		new_position += distance;
		/*
		drawScaledSprite(screen, background.sprite, new_position.x, new_position.y, new_size.x, new_size.y);
		drawScaledSprite(screen, background.sprite, new_position.x - background.size.x, new_position.y, new_size.x, new_size.y);
		drawScaledSprite(screen, background.sprite, new_position.x + background.size.x, new_position.y, new_size.x, new_size.y);
		*/

		if (background.render_type == BackgroundRenderType::HORIZONTAL || background.render_type == BackgroundRenderType::ABOVE)
		{
			float visible_width = window_size.x / screen.getZoom();		
			int tiles = std::ceil(visible_width / background.size.x) + 2;
			int half_tiles = static_cast<int>(std::ceil(tiles / 2.0f));
			for (int i = -half_tiles; i <= half_tiles; ++i)
			{
				float x = new_position.x + i * background.size.x;

				glm::vec2 world_position{ x, background.start_pos.y + offset.y };
				drawScaledSprite(screen, background.sprite, world_position.x, world_position.y, new_size.x, new_size.y);
			
				/*
				if (background.render_type == BackgroundRenderType::ABOVE)
				{
					float visible_height = window_size.y / screen.getZoom();
					int tiles_ = std::ceil(visible_height / background.size.y) + 2;
					int half_tiles_ = static_cast<int>(std::ceil(tiles_ / 2.0f));
					for (int j = -half_tiles_; j <= half_tiles; ++j)
					{
						float y = new_position.y + j * background.size.y;

						if (background.max_height)
							if (y >= background.max_height)
								continue;
							
						glm::vec2 world_position_{ x,y };
						drawScaledSprite(screen, background.sprite, world_position_.x, world_position_.y, new_size.x, new_size.y);
					}
				}*/
			}
		}

		/*if (background.render_type == BackgroundRenderType::HORIZONTAL || background.render_type == BackgroundRenderType::BOTH)
		{
			glm::vec2 new_position2 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance.x - background.size.x + offset.x, background.start_pos.y + offset.y });
			glm::vec2 new_position3 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance.x + background.size.x + offset.x, background.start_pos.y + offset.y });
			drawScaledSprite(screen, background.sprite, new_position2.x, new_position2.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position3.x, new_position3.y, new_size.x, new_size.y);
		}
		if (background.render_type == BackgroundRenderType::VERTICAL || background.render_type == BackgroundRenderType::BOTH)
		{
			glm::vec2 new_position2 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance.x + offset.x, background.start_pos.y + offset.y + background.size.y });
			glm::vec2 new_position3 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + distance.x + offset.x, background.start_pos.y + offset.y - background.size.y});
			drawScaledSprite(screen, background.sprite, new_position2.x, new_position2.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position3.x, new_position3.y, new_size.x, new_size.y);
		}
		if (background.render_type == BackgroundRenderType::BOTH)
		{
			glm::vec2 new_position2 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x - background.size.x + distance.x +offset.x, background.start_pos.y + offset.y - background.size.y });
			glm::vec2 new_position3 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + background.size.x + distance.x + offset.x, background.start_pos.y + offset.y + background.size.y});
			glm::vec2 new_position4 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x - background.size.x + distance.x + offset.x, background.start_pos.y + offset.y + background.size.y});
			glm::vec2 new_position5 = getZoomedCoordinates(screen, glm::vec2{ background.start_pos.x + background.size.x + distance.x  + offset.x, background.start_pos.y + offset.y - background.size.y});
			drawScaledSprite(screen, background.sprite, new_position2.x, new_position2.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position3.x, new_position3.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position4.x, new_position3.y, new_size.x, new_size.y);
			drawScaledSprite(screen, background.sprite, new_position5.x, new_position3.y, new_size.x, new_size.y);
		}*/
		/*if (i == 0)
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

		++i;*/
	}

	/*for (const auto& background : cave_backgrounds)
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
	}*/
}

glm::vec2 DynamicBackground::getZoomedCoordinates(const GpuRenderer& screen, const glm::vec2& position)
{
	const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());
	const auto& zoom = screen.getZoom();
	glm::vec2 mid_screen = glm::vec2{window_size * 0.5f};
	return (position - screen.getView() - mid_screen) * zoom + mid_screen;
}
