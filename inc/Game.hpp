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
	
	glm::vec2 view_position = {0.f, 0.f};
private:
	void updateTilemapTarget();
	void updateInput(float dt);
	void updateImGui(float dt);

	bool lock_camera = true;

	float min_zoom = 0.5f;
	float max_zoom = 2.f;
	float zoom = 1.f;

	float camera_move_speed = 1500.f;

	Renderer& screen;
	//SpriteSheet background;
	//SpriteSheet player;
	SpriteSheet tileset;

	TileMap tilemap;
	World world;

	float tilemap_raws = 10.f;
	float tilemap_columns = 10.f;

	EntityManager entity_manager;
	ComponentManager component_manager;

	PhysicsSystem physics_system;
	InputSystem input_system;
	CollisionSystem collision_system;
	JumpSystem jump_system;
	MiningSystem mining_system;
	PlaceSystem place_system;

	Entity player;
};