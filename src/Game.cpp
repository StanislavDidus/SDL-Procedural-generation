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
	//, background(screen, Surface{ "assets/Sprites/bg1.png" }, {320.f, 180.f})
	//, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
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
	forest.frequency = 0.9f;
	forest.amplitude = 0.7f;
	forest.seed = master_seed;
	forest.strength = 1.f;
	forest.height_multiplier = 1.3f;
	forest.octaves = 2;
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
	desert.octaves = 1;
	desert.tile_id = 11;
	biomes.push_back(desert);

	Biome mountain;
	mountain.name = "Mountain";
	mountain.temperature = 0.2f;
	mountain.moisture = 0.4f;
	mountain.frequency = 2.1f;
	mountain.amplitude = 5.1f;
	mountain.seed = mountain_seed;
	mountain.strength = 1.f;
	mountain.height_multiplier = 4.5f;
	mountain.octaves = 5;
	mountain.tile_id = 30;
	biomes.push_back(mountain);

	Biome snow;
	snow.name = "Snow";
	snow.temperature = 0.1f;
	snow.moisture = 0.6f;
	snow.frequency = .8f;
	snow.amplitude = 1.f;
	snow.seed = temperature_seed;
	snow.strength = .9f;
	snow.octaves = 2;
	snow.height_multiplier = 0.1f;
	snow.tile_id = 6;
	biomes.push_back(snow);

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

float zoom = 1.f;

void Game::update(float dt)
{
	zoom += dt * .5f;

	screen.setZoom(zoom);

	auto& window_size = screen.getWindowSize();
	int screen_y_offset = 200;
	
	float tile_width = tilemap.getTileSize().x;
	float begin = std::floor(view_position.x / tile_width);
	float end = std::ceil(view_position.x / tile_width);
	
	screen.setView(view_position);

	tilemap.clear();
	
	for (float x = begin; x < 100.f + end; x += 1.f)
	{
		float x_slope = Noise::fractal1D(noise, 2, x * scale, 1.1f, 1.1f, cliff_seed);
		float moisture = Noise::fractal1D(noise, 1, x * scale, 0.8f, 1.05f, moisture_seed);
		float temperature = Noise::fractal1D(noise, 1, x * scale, 0.85f, 1.1f, temperature_seed);

		float max_weight = -1.f;
		int max_weight_index = -1;


		float combined_height = 0.f;
		float combined_weight = 0.f; 
		for (int i = 0; i < biomes.size(); i++)
		{
			const auto& biome = biomes[i];

			float weight = biome.weight(temperature, moisture) * biome.strength;
			float height = biome.height<ValueNoise>(x * scale);

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
