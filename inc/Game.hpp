#pragma once

#include <SDL3/SDL.h>

#include "Renderer.hpp"
#include "SpriteSheet.hpp"
#include "ValueNoise.hpp"
#include "Biomes.hpp"
#include "TileMap.hpp"
#include "World.hpp"
#include "glm/glm.hpp"
#include <bitset>

class Game
{
public:
	Game(Renderer& screen);
	virtual ~Game();

	void update(float dt);

	void buttonUp(SDL_Keycode key);
	void buttonPressed(SDL_Keycode key);

	static bool isKeyDown(SDL_Keycode key);
	static bool isKey(SDL_Keycode key);
	static bool isKeyUp(SDL_Keycode key);
	


	glm::vec2 view_position = {0.f, 0.f};
private:
	float zoom = 1.f;

	Renderer& screen;
	//SpriteSheet background;
	//SpriteSheet player;
	SpriteSheet tileset;

	TileMap tilemap;
	World world;

	float tilemap_raws = 10.f;
	float tilemap_columns = 10.f;

	//Input
	std::bitset<256> keys;
	static std::bitset<256> pressed;
	static std::bitset<256> held;
	static std::bitset<256> released;
};