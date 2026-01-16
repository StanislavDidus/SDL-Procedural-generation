#pragma once

#include "Renderer.hpp"
#include "SpriteSheet.hpp"
#include "MapRange.hpp"
#include "TileMap.hpp"
#include "World.hpp"
#include "glm/vec2.hpp"
#include "Item.hpp"
#include "Font.hpp"

#include "ECS/Systems.hpp"
#include "UI/UserInterface.hpp"
#include "ItemManager.hpp"
#include "ObjectManager.hpp"
#include "GenerationData.hpp"
#include <memory>

#include "Surface.hpp"

class Game
{
public:
	explicit Game(Renderer& screen);
	virtual ~Game();

	void update(float dt);

	void resizeSprites();
	
	glm::vec2 view_position = {0.f, 0.f};
private:
	void initSystems();
	void initGenerationData();
	void initItems();
	void initTiles();
	void initNoiseSettings();
	void initMapRanges();
	void initBiomes();
	void initObjects();
	void initPlayer();
	void initUserInterface();

	void updateTilemapTarget();
	void updateInput(float dt);
	void updateImGui(float dt);

	void renderWorld();

	bool lock_camera = true;

	float min_zoom = 0.5f;
	float max_zoom = 2.f;
	float zoom = 1.f;

	float camera_move_speed = 1500.f;

	Renderer& screen;
	//SpriteSheet background;
	//SpriteSheet player;
	SpriteSheet tileset;
	SpriteSheet items_spritesheet;
	SpriteSheet object_spritesheet;

	std::unique_ptr<TileMap> tilemap;

	glm::vec2 world_target;
	std::shared_ptr<World> world;

	float tilemap_raws = 10.f;
	float tilemap_columns = 10.f;

	EntityManager entity_manager;
	ComponentManager component_manager;

	//Systems
	std::unique_ptr<PhysicsSystem> physics_system;
	std::unique_ptr<InputSystem> input_system;
	std::shared_ptr<CollisionSystem> collision_system;
	std::unique_ptr<JumpSystem> jump_system;
	std::unique_ptr<MiningTilesSystem> mining_tiles_system;
	std::unique_ptr<MiningObjectsSystem> mining_objects_system;
	std::unique_ptr<PlaceSystem> place_system;
	std::shared_ptr<ItemUsageSystem> item_usage_system;

	Entity player;

	std::vector<Item> items;

	std::shared_ptr<TileManager> tile_manager;
	std::shared_ptr<ObjectManager> object_manager;
	std::shared_ptr<ItemManager> item_manager;

	/*Item apple;
	Item banana;
	Item heal_potion;
	Item regeneration_potion;*/

	UserInterface interface;

	GenerationData generation_data;

	//Text
	Font font;
	Surface text_surface;
	SpriteSheet text;
};
