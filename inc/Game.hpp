#pragma once

#include <SDL3/SDL.h>

#include "Renderer.hpp"
#include "SpriteSheet.hpp"
#include "ValueNoise.hpp"
#include "Biomes.hpp"
#include "TileMap.hpp"
#include "glm/glm.hpp"

class Game
{
public:
	Game(Renderer& screen);
	virtual ~Game();

	void update(float dt);

	void generateSeed();

	float scale = 0.01f;
	float cliffScale = 0.f;
	float cliffAmplitude = 0.f;

	glm::vec2 view_position = {0.f, 0.f};
private:
	std::vector<Biome> biomes;

	Renderer& screen;
	//SpriteSheet background;
	//SpriteSheet player;
	SpriteSheet tileset;

	ValueNoise noise;

	uint32_t master_seed = 0;
	uint32_t desert_seed = 0;
	uint32_t mountain_seed = 0;
	uint32_t cliff_seed = 0;
	uint32_t temperature_seed = 0;
	uint32_t moisture_seed = 0;

	TileMap tilemap;

	float tilemap_raws = 10.f;
	float tilemap_columns = 10.f;
};