#include "Game.hpp"
#include <iostream>

#include <algorithm>

std::bitset<256> Game::pressed{};
std::bitset<256> Game::released{};
std::bitset<256> Game::held{};


Game::Game(Renderer& screen)
	: screen(screen)
	//, background(screen, Surface{ "assets/Sprites/bg1.png" }, {320.f, 180.f})
	//, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
	, tileset(screen, Surface{ "assets/Sprites/tileset.png" }, { 16.f, 16.f }, SDL_SCALEMODE_NEAREST)
	, world(screen)
	, tilemap(world, tileset, 960.f, 540.f, 75.f, 100.f)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });

	world.generateWorld();
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

void Game::update(float dt)
{
	updateInput(dt);

	screen.setZoom(zoom);
	screen.setView(view_position);

	tilemap.render(screen);
}

void Game::buttonUp(SDL_Keycode key)
{
	if(key < 256) keys.reset(key);
}

void Game::buttonPressed(SDL_Keycode key)
{
	if (key < 256) keys.set(key);
}

bool Game::isKeyDown(SDL_Keycode key)
{
	return pressed[key];
}

bool Game::isKey(SDL_Keycode key)
{
	return held[key];
}

bool Game::isKeyUp(SDL_Keycode key)
{
	return released[key];
}

void Game::updateInput(float dt)
{
	if (isKey(SDLK_D))
	{
		view_position.x += 500.f * dt;
	}
	if (isKey(SDLK_A))
	{
		view_position.x -= 500.f * dt;
	}
	if (isKey(SDLK_W))
	{
		view_position.y -= 500.f * dt;
	}
	if (isKey(SDLK_S))
	{
		view_position.y += 500.f * dt;
	}
	if (isKey(SDLK_Q))
	{
		zoom -= 2.f * dt;
		zoom = std::clamp(zoom, 1.5f, 3.f);
	}
	if (isKey(SDLK_E))
	{
		zoom += 2.f * dt;
		zoom = std::clamp(zoom, 1.5f, 3.f);
	}
	if (isKey(SDLK_Z))
	{
		//scale -= 0.01f * dt;
	}
	if (isKey(SDLK_X))
	{
		//scale += 0.01f * dt;
	}
	if (isKeyDown(SDLK_SPACE))
	{
		world.generateWorld();
	}

	pressed = keys & ~held;
	released = held & ~keys;
	held = keys;
}
