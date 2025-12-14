#include "Game.hpp"
#include <iostream>

#include <algorithm>

std::bitset<256> Game::pressed{};
std::bitset<256> Game::released{};
std::bitset<256> Game::held{};

const float BaseWidthScreen = 960.f;
const float BaseHeightScreen = 540.f;

const float TileWidth = 75.f;
const float TileHeight = 75.f;

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}


Game::Game(Renderer& screen)
	: screen(screen)
	//, background(screen, Surface{ "assets/Sprites/bg1.png" }, {320.f, 180.f})
	//, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
	, tileset(screen, Surface{ "assets/Sprites/tileset.png" }, { 16.f, 16.f }, SDL_SCALEMODE_NEAREST)
	, world(screen)
	//, tilemap(world, tileset, 960.f, 540.f, 75.f, 100.f)
	, tilemap(world, tileset, 75.f, 75.f)
	//, mapRange(0.f, 1.f, 0.f, 0.f)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });

	//float min = 100.f;
	//float max = -100.f;
	//float mean = 0.f;

	//float f = 0.02f;
	//float a = 1.f;

	//for (int i = 0; i < 100'000; i++)
	//{
	//	float noise_value = Noise::fractal2D<ValueNoise>(8, i, 0.f, f, a, 1);
	//	float centre = noise_value - 0.5f;
	//	float contrasted = tanh(4.f * centre);
	//	noise_value = (contrasted + 1.f) / 2.f;

	//	if (noise_value < min) min = noise_value;
	//	if (noise_value > max) max = noise_value;
	//	mean += noise_value;
	//}
	//mean /= 100'000.f;

	//std::cout << "ValueNoise: " << std::endl;
	//std::cout << "Min: " << min << std::endl
	//	<< "Max: " << max << std::endl
	//	<< "Mean: " << mean << std::endl;

	//min = 100.f;
	//max = -100.f;
	//mean = 0.f;

	//for (int i = 0; i < 100'000; i++)
	//{
	//	float noise_value = Noise::fractal2D<PerlynNoise>(8, i, 0.f, f, a, 1);
	//	/*float centre = noise_value - 0.5f;
	//	float contrasted = tanh(8.f * centre);
	//	noise_value = (contrasted + 1.f) / 2.f;*/
	//	//noise_value = std::pow(noise_value, .5f);

	//	if (noise_value < min) min = noise_value;
	//	if (noise_value > max) max = noise_value;
	//	mean += noise_value;
	//}
	//mean /= 100'000.f;

	//std::cout << "PerlynNoise: " << std::endl;
	//std::cout << "Min: " << min << std::endl
	//	<< "Max: " << max << std::endl
	//	<< "Mean: " << mean << std::endl;

	/*mapRange.addPoint(0.25f, 1.f);
	mapRange.addPoint(0.5f, 0.f);
	mapRange.addPoint(0.75f, 1.f);*/

	/*std::cout << "0.f - " << mapRange.getValue(0.f) << std::endl;
	std::cout << "0.1f - " << mapRange.getValue(0.1f) << std::endl;
	std::cout << "0.25f - " << mapRange.getValue(0.25f) << std::endl;*/
	//std::cout << "0.37f - " << mapRange.getValue(0.37f) << std::endl;
	/*std::cout << "0.5f - " << mapRange.getValue(0.5f) << std::endl;
	std::cout << "0.62f - " << mapRange.getValue(0.62f) << std::endl;
	std::cout << "0.75f - " << mapRange.getValue(0.75f) << std::endl;
	std::cout << "0.87f - " << mapRange.getValue(0.87f) << std::endl;
	std::cout << "1.f - " << mapRange.getValue(1.f) << std::endl;
	std::cout << "-0.5.f - " << mapRange.getValue(-0.5f) << std::endl;
	std::cout << "1.5f - " << mapRange.getValue(1.f) << std::endl;*/
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

	//zoom = 1.f;


	//const auto& window_size = screen.getWindowSize();

	//float f = 0.02f;
	//float a = 1.f;

	//float min = 100.f;
	//float max = -100.f;

	//for (int x = 0; x < 480; x++)
	//{
	//	for (int y = 0; y < 480; y++)
	//	{
	//		float noise_value = Noise::fractal2D<PerlynNoise>(8, x, y, f, a, 1);
	//		/*float centre = noise_value - 0.5f;
	//		float contrasted = tanh(8.f * centre);
	//		noise_value = (contrasted + 1.f) / 2.f;*/
	//		//noise_value = std::pow(noise_value, 0.5f);

	//		if (noise_value < min) min = noise_value;
	//		if (noise_value > max) max = noise_value;
	//	}
	//}

	//for (int x = 0; x < 480; x++)
	//{
	//	for (int y = 0; y < 480; y++)
	//	{
	//		float noise_value = Noise::fractal2D<PerlynNoise>(8, x, y, f, a, 1);
	//		/*float centre = noise_value - 0.5f;
	//		float contrasted = tanh(8.f * centre);
	//		noise_value = (contrasted + 1.f) / 2.f;*/
	//		//noise_value = std::pow(noise_value, 0.5f);

	//		noise_value = (noise_value - min) / (max - min);
	//		
	//		uint8_t value = static_cast<uint8_t>(mapRange(noise_value, 0.f, 1.f, 0.f, 255.f));

	//		//assert(value >= 0 && value <= 255);

	//		//value = std::clamp(static_cast<int>(value), 0, 255);
	//		Color color{ value, value, value, 255 };
	//		screen.drawPoint(x, y, color);
	//	}
	//}

	/*for (int x = 480; x < 960; x++)
	{
		for (int y = 0; y < 480; y++)
		{
			float noise_value = Noise::fractal2D<ValueNoise>(8, x, y, 0.02f, 1.f, 1);

			uint8_t value = static_cast<uint8_t>(mapRange(noise_value, 0.f, 1.f, 0.f, 255.f));
			Color color{ value, value, value, 255 };
			screen.drawPoint(x, y, color);
		}
	}*/
}

void Game::resizeSprites()
{
	tilemap.setTileSize(screen.getWindowSize().x / BaseWidthScreen * TileWidth, screen.getWindowSize().y / BaseHeightScreen * TileHeight);
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
		view_position.x += 1500.f * dt;
	}
	if (isKey(SDLK_A))
	{
		view_position.x -= 1500.f * dt;
	}
	if (isKey(SDLK_W))
	{
		view_position.y -= 1500.f * dt;
	}
	if (isKey(SDLK_S))
	{
		view_position.y += 1500.f * dt;
	}
	if (isKey(SDLK_Q))
	{
		zoom -= 2.f * dt;
		zoom = std::clamp(zoom, min_zoom, max_zoom);
	}
	if (isKey(SDLK_E))
	{
		zoom += 2.f * dt;
		zoom = std::clamp(zoom, min_zoom, max_zoom);
	}
	if (isKey(SDLK_Z))
	{
		//world.scale -= 0.1f * dt;
		world.cave_threshold -= 0.2f * dt;
		world.clear();
	}
	if (isKey(SDLK_X))
	{
		//world.scale += 0.1f * dt;
		world.cave_threshold += 0.2f * dt;
		world.clear();
	}
	if (isKeyDown(SDLK_SPACE))
	{
		world.generateWorld();
	}

	pressed = keys & ~held;
	released = held & ~keys;
	held = keys;
}
