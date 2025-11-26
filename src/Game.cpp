#include "Game.hpp"
#include <iostream>
#include <random>
#include <algorithm>

namespace
{
	std::random_device rd;
	std::minstd_rand rng(rd());
}

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}


Game::Game(Renderer& screen)
	: screen(screen)
	, background(screen, Surface{ "assets/Sprites/bg1.png" })
	, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
	, tileset(screen, Surface{ "assets/Sprites/tileset.png" }, { 16.f, 16.f }, SDL_SCALEMODE_NEAREST)
	, tilemap(tileset, 960.f, 540.f, 75.f, 100.f)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });

	master_seed = 0;
	temperature_seed = master_seed + 1;
	moisture_seed = master_seed + 2;
	cliff_seed = master_seed + 3;
	desert_seed = master_seed + 4;
	mountain_seed = master_seed + 5;
	
	Biome forest;
	forest.name = "Forest";
	forest.temperature = 0.4f;
	forest.moisture = .4f;
	forest.frequency = 0.8f;
	forest.amplitude = 0.9f;
	forest.seed = master_seed;
	forest.strength = 1.f;
	forest.height_multiplier = 1.f;
	//forest.color = Color::GREEN;
	forest.tile_id = 8;
	biomes.push_back(forest);

	Biome desert;
	desert.name = "Desert";
	desert.temperature = 0.8f;
	desert.moisture = 0.1f;
	desert.frequency = 0.001f;
	desert.amplitude = 0.05f;
	desert.seed = desert_seed;
	desert.strength = 0.9f;
	desert.height_multiplier = .9f;
	//desert.color = Color::YELLOW;
	desert.tile_id = 11;
	biomes.push_back(desert);

	Biome mountain;
	mountain.name = "Mountain";
	mountain.temperature = 0.2f;
	mountain.moisture = 0.5f;
	mountain.frequency = 1.2f;
	mountain.amplitude = 3.1f;
	mountain.seed = mountain_seed;
	mountain.strength = 1.f;
	mountain.height_multiplier = 3.f;
	//mountain.color = Color::GREY;
	mountain.tile_id = 30;
	biomes.push_back(mountain);

	Biome ocean;
	ocean.name = "Ocean";
	ocean.temperature = 0.5f;
	ocean.moisture = 1.f;
	ocean.frequency = .9f;
	ocean.amplitude = 1.5f;
	ocean.seed = temperature_seed;
	ocean.strength = 1.f;
	ocean.height_multiplier = 0.1f;
	//ocean.color = Color::BLUE;
	ocean.tile_id = 6;
	biomes.push_back(ocean);

	//Tileset
	//tilemap.setTile(8, 0, 0);

	for (int i = 0; i < 18; i++)
	{
		//tilemap.setTile(8, 9, i);
	}
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

void Game::update(float dt)
{
	/*angle += dt * 15.f;
	view_pos_x += dt * 25.f;

	screen.setView({view_pos_x, 0.f});


	glm::ivec2 player_size = { window_size.x / 2.f, window_size.y / 1.f };

	screen.drawLine(0, 0, window_size.x, window_size.y, Color::RED);
	screen.drawRectangle(1.f, 1.f, 200.f, 150.f, RenderType::FILL, Color::YELLOW);
	screen.drawPoint(400, 250, Color::BLUE);
	screen.drawUI(background, 0.f, 0.f, 400.f, 400.f);
	player.setFrame(12);
	screen.drawScaledSprite(player, window_size.x / 2 - player_size.x / 2, window_size.y / 2 - player_size.y / 2, player_size.x, player_size.y);*/

	tilemap.clear();

	auto& window_size = screen.getWindowSize();

	screen.setView(view_position);

	int screen_y_offset = 200;

	float hugeMountainBeginX = 600.f;
	float hugeMountainEndX = 800.f;

	bool was_desert = false;
	for (float x = view_position.x / tilemap.getTileSize().x; x < 100.f + view_position.x / tilemap.getTileSize().x; x += 1.f)
	{
		float x_slope = Noise::fractal1D(noise, 2, x * scale, 1.1f, 1.1f, cliff_seed);
		float moisture = Noise::fractal1D(noise, 1, x * scale, 0.8f, 0.9f, moisture_seed);
		float temperature = Noise::fractal1D(noise, 1, x * scale, 0.7f, 0.7f, temperature_seed);

		float max_weight = -1.f;
		int max_weight_index = -1;


		float combined_height = 0.f;
		float combined_weight = 0.f; 
		for (int i = 0; i < biomes.size(); i++)
		{
			const auto& biome = biomes[i];

			//float height = Noise::fractal1D(noise, 3, x * scale, biome.frequency, biome.amplitude, biome.seed) * biome.height_multiplier;
			float weight = biome.weight(temperature, moisture) * biome.strength;
			float height = Noise::fractal1D(noise, 3, x * scale, biome.frequency, biome.amplitude, biome.seed) * biome.height_multiplier;

			combined_height += height * weight;
			combined_weight += weight;

			if (weight > max_weight)
			{
				max_weight = weight;
				max_weight_index = i;
			}
		}
		combined_height /= combined_weight;
		int tile_id = biomes[max_weight_index].tile_id;

		//Draw offset
		int x_offset = static_cast<int>(mapRange(x_slope, 0.f, 1.f, -25.f, 25.f));
		int y_offset = static_cast<int>(mapRange(combined_height, 0.f, 1.f, -25.f, 25.f));

		//assert(between(blend_height, 0.f, 1.f));
		
		//Render
		//screen.drawPoint(static_cast<int>(x) + x_offset, screen_y_offset - y_offset, Color::WHITE);

		tilemap.setTile(tile_id, y_offset, x);
	}

	tilemap.render(screen);

	//std::cout << scale << "\n";
}

void Game::generateSeed()
{
	std::uniform_int_distribution dist(0, 10000000);
	master_seed = dist(rng);

	temperature_seed = master_seed + 1;
	moisture_seed = master_seed + 2;
	cliff_seed = master_seed + 3;
	desert_seed = master_seed + 4;
	mountain_seed = master_seed + 5;

	biomes[0].seed = master_seed;
	biomes[1].seed = desert_seed;
	biomes[2].seed = mountain_seed;
}
