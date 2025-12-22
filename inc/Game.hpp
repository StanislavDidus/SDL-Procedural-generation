#pragma once

#include <SDL3/SDL.h>

#include "Renderer.hpp"
#include "SpriteSheet.hpp"
#include "ValueNoise.hpp"
#include "PerlynNoise.hpp"
#include "MapRange.hpp"
#include "Biomes.hpp"
#include "TileMap.hpp"
#include "World.hpp"
#include "glm/glm.hpp"
#include <bitset>

#include "ECS/Systems.hpp"

class Game
{
public:
	Game(Renderer& screen);
	virtual ~Game();

	void update(float dt);

	void resizeSprites();

	void buttonUp(SDL_Keycode key);
	void buttonPressed(SDL_Keycode key);

	static bool isKeyDown(SDL_Keycode key);
	static bool isKey(SDL_Keycode key);
	static bool isKeyUp(SDL_Keycode key);
	
	glm::vec2 view_position = {0.f, 0.f};
private:
	void updateInput(float dt);
	void updateImGui(float dt);

	float min_zoom = .05f;
	float max_zoom = 1.f;
	float zoom = (min_zoom + max_zoom) / 2.f;

	float camera_move_speed = 1500.f;

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

	EntityManager entity_manager;
	ComponentManager component_manager;
	PhysicsSystem physics_system;
	Entity player;
};