#include "Game.hpp"
#include <iostream>

Game::Game(Renderer& screen)
	: screen(screen)
	, background(screen, Surface{ "assets/Sprites/bg1.png" })
	, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

float view_pos_x = 0.f;
float angle = 0.f;

void Game::update(float dt)
{
	angle += dt * 15.f;
	view_pos_x += dt * 25.f;

	screen.setView({view_pos_x, 0.f});

	auto& window_size = screen.getWindowSize();
	glm::ivec2 player_size = { window_size.x / 2.f, window_size.y / 1.f };

	screen.drawLine(0, 0, window_size.x, window_size.y, Color::RED);
	screen.drawRectangle(1.f, 1.f, 200.f, 150.f, RenderType::FILL, Color::YELLOW);
	screen.drawPoint(400, 250, Color::BLUE);
	screen.drawUI(background, 0.f, 0.f, 400.f, 400.f);
	player.setFrame(12);
	screen.drawScaledSprite(player, window_size.x / 2 - player_size.x / 2, window_size.y / 2 - player_size.y / 2, player_size.x, player_size.y);
}
