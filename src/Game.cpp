#include "Game.hpp"
#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "InputManager.hpp"

#include <random>

#include <algorithm>

namespace
{
	std::random_device rd;
	std::minstd_rand rng(rd());
}

Game::Game(Renderer& screen)
	: screen(screen)
	//, background(screen, Surface{ "assets/Sprites/bg1.png" }, {320.f, 180.f})
	//, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
	, tileset(screen, Surface{ "assets/Sprites/tileset.png" }, { 16.f, 16.f }, SDL_SCALEMODE_NEAREST)
	, items_spritesheet(screen, Surface{ "assets/Sprites/items.png" }, {16.f, 16.f}, SDL_SCALEMODE_NEAREST)
	, object_spritesheet(screen, Surface{ "assets/Sprites/objects.png" }, {16.f, 48.f}, SDL_SCALEMODE_NEAREST)
	//, tilemap(world, tileset, 960.f, 540.f, 75.f, 100.f)
	, font("assets/Fonts/Roboto-Black.ttf", 32)
	, text_surface(font.getFont(), "Player", {0,0,0,0})
	, text(screen, text_surface)
	/*, health_bar({ 0.f, screen.getWindowSize().y - 50.f, }, {250.f, 50.f}, component_manager.health[player].current_health, 100.f, Color::RED)*/
	//, mapRange(0.f, 1.f, 0.f, 0.f)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });

	
	item_usage_system = std::make_shared<ItemUsageSystem>(component_manager, player);
	item_manager = std::make_shared<ItemManager>();

	initItems();
	initNoiseSettings();
	initMapRanges();
	initTiles();
	initObjects();
	initGenerationData();
	initBiomes();

	world = std::make_shared<World>(generation_data, tileset, object_spritesheet, collision_system, object_manager, 500, 200, 20.f, 20.f);

	initSystems();
	initPlayer();	
	initUserInterface();

	world->generateWorld(0);

	//tilemap = std::make_unique<TileMap>(*world, tileset, object_spritesheet, collision_system, 20.f, 20.f);

	auto& inventory = component_manager.has_inventory[player].inventory;
	inventory->addItem(0, 15);
	inventory->addItem(1, 1);
	inventory->addItem(2, 1);
	//inventory.removeItem(1);
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

void Game::initSystems()
{
	physics_system = std::make_unique<PhysicsSystem>(component_manager, entity_manager);
	input_system = std::make_unique<InputSystem>(component_manager, entity_manager);
	collision_system = std::make_shared<CollisionSystem>(component_manager, entity_manager);
	jump_system = std::make_unique<JumpSystem>(component_manager, entity_manager);
	mining_tiles_system = std::make_unique<MiningTilesSystem>(component_manager, entity_manager, *world, 20.f, 20.f);
	mining_objects_system = std::make_unique<MiningObjectsSystem>(component_manager, entity_manager, *world, object_manager, 20.f, 20.f);
	place_system = std::make_unique<PlaceSystem>(component_manager, entity_manager, *world, 20.f, 20.f);
	item_usage_system = std::make_shared<ItemUsageSystem>(component_manager, player);

	world->setCollisionSystem(collision_system);
}

void Game::initGenerationData()
{
	generation_data.y_base = 25.f;
	generation_data.cave_y_base = 10.f;
	generation_data.sea_y_base = 19.f;
	generation_data.scale = 0.5f;
	generation_data.density_threshold = 0.5f;
	generation_data.tree_threshold = 0.65f;

	generation_data.tile_manager = tile_manager;
	generation_data.object_manager = object_manager;
}

void Game::initItems()
{
	item_manager = std::make_shared<ItemManager>();
	
	//Apple
	{
		std::vector<std::shared_ptr<ItemComponent>> components;
		components.push_back(std::make_shared<ItemComponents::Usable>());
		components.push_back(std::make_shared<ItemComponents::Heal>(10));
		ItemProperties properties{ true, 0, "Apple", components };

		item_manager->addItem(properties);
	}

	//Banana
	{
		std::vector<std::shared_ptr<ItemComponent>> components;
		components.push_back(std::make_shared<ItemComponents::Usable>());
		components.push_back(std::make_shared<ItemComponents::Heal>(5));
		ItemProperties properties{ true, 1, "Banana", components };

		item_manager->addItem(properties);
	}

	//Heal Potion
	{
		std::vector<std::shared_ptr<ItemComponent>> components;
		components.push_back(std::make_shared<ItemComponents::Usable>());
		components.push_back(std::make_shared<ItemComponents::Heal>(50));
		ItemProperties properties{ true, 2, "Heal_Potion", components };

		item_manager->addItem(properties);
	}

	//Regeneration Potion
	{
		std::vector<std::shared_ptr<ItemComponent>> components;
		components.push_back(std::make_shared<ItemComponents::Usable>());
		components.push_back(std::make_shared<ItemComponents::AddEffect>(Effect::HEALTH_REGENERATION, 120.f));
		ItemProperties properties{ true, 3, "Regeneration_Potion", components };

		item_manager->addItem(properties);
	}

	//Wood
	{
		std::vector<std::shared_ptr<ItemComponent>> components;
		ItemProperties properties{ true, 4, "Wood", components };
		item_manager->addItem(properties);
	}
}

void Game::initObjects()
{
	object_manager = std::make_shared<ObjectManager>();

	//Tree
	{
		RandomizedItem apple{ 0, 0.5f, 1, 1 };
		RandomizedItem wood{ 4, 1.f, 2, 7 };
		std::vector<RandomizedItem> items{ apple, wood };
		ObjectProperties properties{ 200.f, 0, "Tree", items, glm::vec2{20.f, 60.f} };
		object_manager->addObject(properties);
	}

	//Snow Tree
	{
		RandomizedItem apple{ 0, 0.5f, 1, 1 };
		RandomizedItem wood{ 4, 1.f, 2, 7 };
		std::vector<RandomizedItem> items{ apple, wood };
		ObjectProperties properties{ 200.f, 1, "Snow_Tree", items, glm::vec2{20.f, 60.f} };
		object_manager->addObject(properties);
	}

	//Cactus
	{
		RandomizedItem apple{ 0, 0.5f, 1, 1 };
		RandomizedItem wood{ 4, 1.f, 2, 7 };
		std::vector<RandomizedItem> items{ apple, wood };
		ObjectProperties properties{ 200.f, 2, "cactus", items, glm::vec2{20.f, 60.f} };
		object_manager->addObject(properties);
	}
}


void Game::initTiles()
{
	tile_manager = std::make_shared<TileManager>();

	//Grass
	{
		TileProperties properties{ 0, TileType::SURFACE, true, 100.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::GRASS] = id;
	}

	//Dirt
	{
		TileProperties properties{ 1, TileType::DIRT, true, 100.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::DIRT] = id;
	}

	//Stone
	{
		TileProperties properties{ 2, TileType::STONE, true, 125.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::STONE] = id;
	}

	//Sand
	{
		TileProperties properties{ 3, TileType::SURFACE, true, 50.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::SAND] = id;
	}

	//Sky
	{
		TileProperties properties{ 4, TileType::NONE, false, 0.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::SKY] = id;
	}

	//Snow Grass
	{
		TileProperties properties{ 5, TileType::SURFACE, true, 100.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::SNOW_GRASS] = id;
	}

	//Snow Dirt
	{
		TileProperties properties{ 8, TileType::DIRT, true, 100.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::SNOW_DIRT] = id;
	}

	//Rock
	{
		TileProperties properties{ 6, TileType::STONE, true, 125.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::ROCK] = id;
	}

	//Water
	{
		TileProperties properties{ 7, TileType::NONE, false, 0.f };
		int id = tile_manager->addTile(properties);
		generation_data.tiles[BlockType::WATER] = id;
	}
}

void Game::initNoiseSettings()
{
	//PV
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::PV];
		settings.octaves = 5;
		settings.frequency = 0.05f;
		settings.amplitude = 1.0f;
	}

	//Density
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::DENSITY];
		settings.octaves = 8;
		settings.frequency = 0.75f;
		settings.amplitude = 1.0f;
	}

	//Dirt
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::DIRT];
		settings.octaves = 1;
		settings.frequency = 0.2f;
		settings.amplitude = 1.0f;
	}

	//Cave
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::CAVE];
		settings.octaves = 8;
		settings.frequency = 0.1f;
		settings.amplitude = 1.0f;
	}

	//Tunnel
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::TUNNEL];
		settings.octaves = 8;
		settings.frequency = 0.1f;
		settings.amplitude = 1.0f;
	}

	//Temperature
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::TEMPERATURE];
		settings.octaves = 1;
		settings.frequency = 0.05f;
		settings.amplitude = 1.0f;
	}

	//Moisture
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::MOISTURE];
		settings.octaves = 2;
		settings.frequency = 0.04f;
		settings.amplitude = 1.2f;
	}

	//Trees
	{
		NoiseSettings& settings = generation_data.noise_settings[NoiseType::TREES];
		settings.octaves = 2;
		settings.frequency = 0.7f;
		settings.amplitude = 1.5f;
	}
}

void Game::initMapRanges()
{
	//PV_HEIGHT
	{
		MapRange map{ 0.4f, 1.8f, 0.f, -25.f };
		generation_data.maps[MapRangeType::PV_HEIGHT] = map;
	}

	//PV_CHANGE
	{
		MapRange map{ 0.4f, 1.8f, 0.9f, 0.15f };
		generation_data.maps[MapRangeType::PV_CHANGE] = map;
	}

	//CAVE_HEIGHT
	{
		MapRange map{ generation_data.cave_y_base, generation_data.cave_y_base + 300.f, 0.15f, 0.5f };

		map.addPoint(generation_data.cave_y_base + 25.f, 0.25f);

		generation_data.maps[MapRangeType::CAVE_CHANGE] = map;
	}
}

void Game::initBiomes()
{
	//Forest
	{
		Biome& biome = generation_data.biomes[BiomeType::FOREST];
		biome.name = "Forest";
		biome.pv_min = 0.0f;
		biome.pv_max = 1.0f;
		biome.temperature_min = 0.4f;
		biome.temperature_max = 0.8f;
		biome.moisture_min = 0.f;
		biome.moisture_max = 1.0f;
		biome.surface_tile = generation_data.tiles.at(BlockType::GRASS);
		biome.dirt_tile = generation_data.tiles.at(BlockType::DIRT);
	}

	//Tundra
	{
		Biome& biome = generation_data.biomes[BiomeType::TUNDRA];
		biome.name = "Tundra";
		biome.pv_min = 0.f;
		biome.pv_max = 1.f;
		biome.temperature_min = 0.0f;
		biome.temperature_max = 0.4f;
		biome.moisture_min = 0.f;
		biome.moisture_max = 1.f;
		biome.surface_tile = generation_data.tiles.at(BlockType::SNOW_GRASS);
		biome.dirt_tile = generation_data.tiles.at(BlockType::SNOW_DIRT);
	}

	//Desert
	{
		Biome& biome = generation_data.biomes[BiomeType::DESERT];
		biome.name = "Desert";
		biome.pv_min = 0.0f;
		biome.pv_max = 0.6f;
		biome.temperature_min = 0.8f;
		biome.temperature_max = 1.0f;
		biome.moisture_min = 0.0f;
		biome.moisture_max = 1.0f;
		biome.surface_tile = generation_data.tiles.at(BlockType::SAND);
		biome.dirt_tile = generation_data.tiles.at(BlockType::SAND);
	}
}

void Game::initPlayer()
{
	player = entity_manager.createEntity().value();

	component_manager.transform[player] = Transform{
	glm::vec2{400.f, -500.f},
	glm::vec2{25.f, 60.f}
	};

	component_manager.physics[player] = Physics{
		true,
	glm::vec2{0.f, 0.f},
	glm::vec2{1500.f, 0.f},
	glm::vec2{250.f, 200.f,},
	5.f,
	false,
	20.f
	};

	component_manager.jump[player] = Jump{
		475.f,
		false
	};

	component_manager.player[player] = Player{

	};

	component_manager.mine_intent[player] = MineIntent
	{
		glm::vec2{},
		glm::vec2{},
		false
	};

	component_manager.mine_tiles_ability[player] = MineTilesAbility
	{
		150.f,
		200.f,
		5
	};

	component_manager.mine_objects_ability[player] = MineObjectsAbility
	{
		150.f,
		200.f,
	};

	component_manager.place_ability[player] = PlaceAbility
	{
		200.f,
		0.3f,
		0.0f
	};

	component_manager.place_intent[player] = PlaceIntent
	{
		glm::vec2{},
		false
	};

	component_manager.has_inventory[player] = HasInventory
	{
		std::make_shared<Inventory>(item_usage_system, item_manager, 15)
	};

	component_manager.health[player] = Health
	{
		100.f,
		50.f
	};
}

void Game::initUserInterface()
{
	interface.addFillBar({ 0.f, screen.getWindowSize().y - 50.f, }, { 250.f, 50.f }, component_manager.health[player].current_health, 100.f, Color::RED);

	interface.addInventoryView(font, items_spritesheet, component_manager.has_inventory[player].inventory.get(), 3, 5, 50.f, {0.f, 0.f});
}

void Game::update(float dt)
{
	//Update
	updateInput(dt);

	screen.setZoom(zoom);
	screen.setView(view_position);

	//Check mouse state
	const auto& mouse = InputManager::getMouseState();

	interface.update();

	input_system->update(screen, dt);
	jump_system->update(dt);
	physics_system->update(dt);
	collision_system->update(dt);

	if (!interface.isMouseCoveringInventory())
	{
		mining_tiles_system->update(dt);
		place_system->update(dt);
		mining_objects_system->update(dt);
	}

	world->update(screen, dt, world_target);
	world->updateTiles();
	world->rebuildChunks();

	updateTilemapTarget();

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// … your UI code
	updateImGui(dt);
	
	//Render
	const auto& window_size = screen.getWindowSize();

	collision_system->collisions.clear();
	world->render(screen);

	mining_tiles_system->renderOutline(screen);

	auto& pos = component_manager.transform[player].position;
	auto& size = component_manager.transform[player].size;

	screen.drawRectangle(pos.x, pos.y, size.x, size.y, RenderType::FILL, Color::RED);

	mining_objects_system->render(screen);
	interface.render(screen);

	const auto& player_pos = component_manager.transform[player].position;
	const auto& player_size = component_manager.transform[player].size;
	screen.printText(text.getTexture(), player_pos.x, player_pos.y - 30.f, player_size.x, 30.f);

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), screen.getRenderer());
}

void Game::resizeSprites()
{
	/*const auto& window_size = screen.getWindowSize();
	float related_width = window_size.x / BaseWidthScreen;
	float related_height = window_size.y / BaseHeightScreen;*/

	//component_manager.transform[player].size = { related_width * PlayerWidth, related_height * PlayerHeight };
	//tilemap.setTileSize(related_width * TileWidth, related_height * TileHeight);
}

void Game::updateTilemapTarget()
{
	if (lock_camera)
	{
		//Set target on player
		const auto& player_transform = component_manager.transform[player];
		const auto& player_position = player_transform.position;
		const auto& player_size = player_transform.size;
		world_target = { player_position.x + player_size.x / 2.f, player_position.y + player_size.y / 2.f };
		//glm::vec2 player_screen_position = world_target - static_cast<glm::vec2>(screen.getWindowSize()) / 2.f;
		view_position = world_target;
	}
	else
	{
		const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());

		glm::vec2 view_center = { view_position.x + window_size.x / 2.f, view_position.y + window_size.y / 2.f };

		world_target = view_center;
	}
}

void Game::updateInput(float dt)
{
	if (!lock_camera)
	{
		if (InputManager::isKey(SDLK_D))
		{
			view_position.x += camera_move_speed * dt;
		}
		if (InputManager::isKey(SDLK_A))
		{
			view_position.x -= camera_move_speed * dt;
		}
		if (InputManager::isKey(SDLK_W))
		{
			view_position.y -= camera_move_speed * dt;
		}
		if (InputManager::isKey(SDLK_S))
		{
			view_position.y += camera_move_speed * dt;
		}
	}
	if (InputManager::isKey(SDLK_Q))
	{
		zoom -= 2.f * dt;
		zoom = std::clamp(zoom, min_zoom, max_zoom);
	}
	if (InputManager::isKey(SDLK_E))
	{
		zoom += 2.f * dt;
		zoom = std::clamp(zoom, min_zoom, max_zoom);
	}
	if (InputManager::isKey(SDLK_Z))
	{
		//world->scale -= 0.1f * dt;
		//world->cave_threshold -= 0.2f * dt;
		
	}
	if (InputManager::isKey(SDLK_X))
	{
		//world->scale += 0.1f * dt;
		//world->cave_threshold += 0.2f * dt;
		
	}
	if (InputManager::isKeyDown(SDLK_SPACE))
	{
		world->generateWorld(std::nullopt);
	}

	//Add items
	if (InputManager::isKeyDown(SDLK_G))
	{
		component_manager.has_inventory[player].inventory->addItem(1,1);
	}

	//Change the mining radius
	if (InputManager::isKeyDown(SDLK_EQUALS))
	{
		//component_manager.mine_ability[player].mine_size++;
		
	}
	if (InputManager::isKeyDown(SDLK_MINUS))
	{
		//int& mine_size = component_manager.mine_ability[player].mine_size;
		//mine_size--;
		//mine_size = std::max(1, mine_size);

	}
}

void Game::updateImGui(float dt)
{
	ImGui::Begin("Window");

	if (ImGui::CollapsingHeader("Player"))
	{
		ImGui::Text("Tiles");
		if (component_manager.mine_tiles_ability.contains(player))
		{
			ImGui::SliderFloat("mining speed", &component_manager.mine_tiles_ability[player].speed, 0.f, 1000.f);
			ImGui::SliderFloat("mining radius", &component_manager.mine_tiles_ability[player].radius, 0.f, 500.f);
		}
		else
		{
			ImGui::Text("*Component is missing*");
		}

		ImGui::Text("Objects");
		if (component_manager.mine_objects_ability.contains(player))
		{
			ImGui::SliderFloat("mining speed", &component_manager.mine_objects_ability[player].speed, 0.f, 1000.f);
			ImGui::SliderFloat("mining radius", &component_manager.mine_objects_ability[player].radius, 0.f, 500.f);
		}
		else
		{
			ImGui::Text("*Component is missing*");
		}
		
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SliderFloat("camera_move_speed", &camera_move_speed, 1000.f, 10000.f);
		ImGui::SliderFloat("zoom", &zoom, 0.1f, 5.f);

		ImGui::Checkbox("Lock camera on player", &lock_camera);
	}

	if (ImGui::CollapsingHeader("Procedural generation"))
	{
		static int buffer = 0;
		ImGui::InputInt("Seed", &buffer);

		if (ImGui::Button("Random seed"))
		{
			std::uniform_int_distribution dist(0, 10000000);
			buffer = dist(rng);
		}

		ImGui::SameLine();

		if (ImGui::Button("Generate world"))
		{
			world->generateWorld(buffer);
		}

		ImGui::SameLine();

		if (ImGui::Button("Generate world with random seed"))
		{
			std::uniform_int_distribution dist(0, 10000000);
			buffer = dist(rng);
			world->generateWorld(buffer);
		}

		//ImGui::SliderFloat("scale", &world->scale, 0.f, 1.f);
		//ImGui::SliderFloat("density_change", &world->density_change, 0.1f, 1.f);
		//ImGui::SliderFloat("y_base", &world->y_base, -100.f, 100.f);
		//ImGui::SliderFloat("sea_level", &world->sea_level, -100.f, 100.f);

		//ImGui::SliderFloat("cave_threshold_min", &world->cave_threshold_min, 0.1f, 1.0f);
		//ImGui::SliderFloat("cave_threshold_max", &world->cave_threshold_max, 0.1f, 1.0f);
		//ImGui::SliderFloat("cave_threshold_step", &world->cave_threshold_step, 0.0001f, 0.1f);
		//ImGui::SliderFloat("cave_base_y", &world->cave_base_height, -100.f, 100.f);

		static int current = 0;
		const char* items[] = { "Default", "PV", "Temperature", "Moisture", "Durability"};

		//ImGui::Combo("Render Mode", &tilemap->render_mode, items, IM_ARRAYSIZE(items));

		/*if (ImGui::CollapsingHeader("Terrain & Nature"))
		{
			ImGui::Text("PV");
			ImGui::SliderInt("Octaves##PV", &world->peaks_and_valleys_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##PV", &world->peaks_and_valleys_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##PV", &world->peaks_and_valleys_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Density");
			ImGui::SliderInt("Octaves##Density", &world->density_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Density", &world->density_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Density", &world->density_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Caves");
			ImGui::SliderInt("Octaves##Caves", &world->cave_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Caves", &world->cave_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Caves", &world->cave_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Tunnels");
			ImGui::SliderInt("Octaves##Tunnels", &world->tunnel_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Tunnels", &world->tunnel_settings.frequency, 0.001f, 2.f);
			ImGui::SliderFloat("Amplitude##Tunnels", &world->tunnel_settings.amplitude, 0.001f, 2.f);

			ImGui::Text("Temperature");
			ImGui::SliderInt("Octaves##Temperature", &world->temperature_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Temperature", &world->temperature_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Temperature", &world->temperature_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Moisture");
			ImGui::SliderInt("Octaves##Moisture", &world->moisture_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Moisture", &world->moisture_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Moisture", &world->moisture_settings.amplitude, 0.0001f, 2.f);
		}*/
	}

	ImGui::End();
}

void Game::renderWorld()
{
	
}
