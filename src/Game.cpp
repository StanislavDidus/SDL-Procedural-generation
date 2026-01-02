#include "Game.hpp"
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
	, world(screen)
	//, tilemap(world, tileset, 960.f, 540.f, 75.f, 100.f)
	, physics_system(component_manager, entity_manager)
	, input_system(component_manager, entity_manager)
	, collision_system(component_manager, entity_manager)
	, jump_system(component_manager, entity_manager)
	, mining_system(component_manager, entity_manager, world, 20.f, 20.f)
	, place_system(component_manager, entity_manager, world, 20.f, 20.f)
	, tilemap(world, tileset, collision_system, 20.f, 20.f)
	, font("assets/Fonts/Roboto-Black.ttf", 32)
	, text_surface(font.getFont(), "Player", {0,0,0,0})
	, text(screen, text_surface)
	/*, health_bar({ 0.f, screen.getWindowSize().y - 50.f, }, {250.f, 50.f}, component_manager.health[player].current_health, 100.f, Color::RED)*/
	//, mapRange(0.f, 1.f, 0.f, 0.f)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });

	initItems();
	initPlayer();	
	initUserInterface();

	auto& inventory = component_manager.has_inventory[player].inventory;
	inventory.addItem(apple);
	inventory.addItem(apple);
	inventory.addItem(apple);
	inventory.addItem(apple);
	for (int i = 0; i < 10; i++)
	{
		inventory.addItem(apple);
	}
	inventory.addItem(banana);
	inventory.addItem(heal_potion);
	inventory.removeItem(1);
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

void Game::initItems()
{
	apple.properties = ItemProperties{ true, 1, 0, "Apple" };
	apple.components.push_back(std::make_shared<ItemComponents::Heal>(10));

	banana.properties = ItemProperties{ true, 1, 1, "Banana" };
	banana.components.push_back(std::make_shared<ItemComponents::Heal>(5));

	heal_potion.properties = ItemProperties{ true, 1, 2, "Heal_Potion" };
	heal_potion.components.push_back(std::make_shared<ItemComponents::Heal>(50));

	regeneration_potion.properties = ItemProperties{ true, 1, 3, "Regeneration_Potion" };
	regeneration_potion.components.push_back(std::make_shared<ItemComponents::AddEffect>(Effect::HEALTH_REGENERATION, 120.f));
}

void Game::initPlayer()
{
	player = entity_manager.createEntity().value();

	component_manager.transform[player] = Transform{
	glm::vec2{400.f, -500.f},
	glm::vec2{25.f, 60.f}
	};

	component_manager.physics[player] = Physics{
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

	component_manager.mine_ability[player] = MineAbility
	{
		150.f,
		200.f,
		5
	};

	component_manager.mine_intent[player] = MineIntent
	{
		false
	};

	component_manager.place_ability[player] = PlaceAbility
	{
		200.f,
		0.3f,
		0.0f
	};

	component_manager.place_intent[player] = PlaceIntent
	{
		false
	};

	component_manager.has_inventory[player] = HasInventory
	{
		Inventory{15},
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

	interface.addInventoryView(font, items_spritesheet, &component_manager.has_inventory[player].inventory, 3, 5, 50.f, {0.f, 0.f});
}

void Game::update(float dt)
{
	//Update
	updateInput(dt);

	screen.setZoom(zoom);
	screen.setView(view_position);

	

	//Check mouse state
	const auto& mouse = InputManager::getMouseState();

	input_system.update(dt);
	jump_system.update(dt);
	physics_system.update(dt);
	collision_system.update(dt);
	mining_system.update(dt, mouse, screen);
	place_system.update(dt, mouse, screen);

	world.updateTiles();

	interface.update();

	updateTilemapTarget();

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// … your UI code
	updateImGui(dt);
	
	//Render
	const auto& window_size = screen.getWindowSize();

	tilemap.render(screen);

	mining_system.renderOutline(dt, mouse, screen);

	auto& pos = component_manager.transform[player].position;
	auto& size = component_manager.transform[player].size;
	if(component_manager.physics[player].is_ground)
		screen.drawRectangle(pos.x, pos.y, size.x, size.y, RenderType::FILL, Color::RED);
	else if(!component_manager.physics[player].is_ground)
		screen.drawRectangle(pos.x, pos.y, size.x, size.y, RenderType::FILL, Color::BLUE);

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
		glm::vec2 player_center = { player_position.x + player_size.x / 2.f, player_position.y + player_size.y / 2.f };
		glm::vec2 player_screen_position = player_center - static_cast<glm::vec2>(screen.getWindowSize()) / 2.f;
		view_position = player_screen_position;
		tilemap.setTarget(player_center);
	}
	else
	{
		const auto& window_size = static_cast<glm::vec2>(screen.getWindowSize());

		glm::vec2 view_center = { view_position.x + window_size.x / 2.f, view_position.y + window_size.y / 2.f };

		tilemap.setTarget(view_center);
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
		//world.scale -= 0.1f * dt;
		world.cave_threshold -= 0.2f * dt;
		world.clear();
	}
	if (InputManager::isKey(SDLK_X))
	{
		//world.scale += 0.1f * dt;
		world.cave_threshold += 0.2f * dt;
		world.clear();
	}
	if (InputManager::isKeyDown(SDLK_SPACE))
	{
		world.generateWorld(std::nullopt);
	}

	//Add items
	if (InputManager::isKeyDown(SDLK_G))
	{
		component_manager.has_inventory[player].inventory.addItem(banana);
	}

	//Change the mining radius
	if (InputManager::isKeyDown(SDLK_EQUALS))
	{
		component_manager.mine_ability[player].mine_size++;
		
	}
	if (InputManager::isKeyDown(SDLK_MINUS))
	{
		int& mine_size = component_manager.mine_ability[player].mine_size;
		mine_size--;
		mine_size = std::max(1, mine_size);

	}
}

void Game::updateImGui(float dt)
{
	ImGui::Begin("Window");

	if (ImGui::CollapsingHeader("Player"))
	{
		ImGui::SliderFloat("mining speed", &component_manager.mine_ability[player].speed, 0.f, 1000.f);
		ImGui::SliderFloat("mining radius", &component_manager.mine_ability[player].radius, 0.f, 500.f);
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
			world.generateWorld(buffer);
		}

		ImGui::SameLine();

		if (ImGui::Button("Generate world with random seed"))
		{
			std::uniform_int_distribution dist(0, 10000000);
			buffer = dist(rng);
			world.generateWorld(buffer);
		}

		ImGui::SliderFloat("scale", &world.scale, 0.f, 1.f);
		ImGui::SliderFloat("density_change", &world.density_change, 0.1f, 1.f);
		ImGui::SliderFloat("y_base", &world.y_base, -100.f, 100.f);
		ImGui::SliderFloat("sea_level", &world.sea_level, -100.f, 100.f);

		ImGui::SliderFloat("cave_threshold_min", &world.cave_threshold_min, 0.1f, 1.0f);
		ImGui::SliderFloat("cave_threshold_max", &world.cave_threshold_max, 0.1f, 1.0f);
		ImGui::SliderFloat("cave_threshold_step", &world.cave_threshold_step, 0.0001f, 0.1f);
		ImGui::SliderFloat("cave_base_y", &world.cave_base_height, -100.f, 100.f);

		static int current = 0;
		const char* items[] = { "Default", "PV", "Temperature", "Moisture", "Durability"};

		ImGui::Combo("Render Mode", &tilemap.render_mode, items, IM_ARRAYSIZE(items));

		if (ImGui::CollapsingHeader("Terrain & Nature"))
		{
			ImGui::Text("PV");
			ImGui::SliderInt("Octaves##PV", &world.peaks_and_valleys_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##PV", &world.peaks_and_valleys_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##PV", &world.peaks_and_valleys_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Density");
			ImGui::SliderInt("Octaves##Density", &world.density_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Density", &world.density_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Density", &world.density_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Caves");
			ImGui::SliderInt("Octaves##Caves", &world.cave_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Caves", &world.cave_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Caves", &world.cave_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Tunnels");
			ImGui::SliderInt("Octaves##Tunnels", &world.tunnel_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Tunnels", &world.tunnel_settings.frequency, 0.001f, 2.f);
			ImGui::SliderFloat("Amplitude##Tunnels", &world.tunnel_settings.amplitude, 0.001f, 2.f);

			ImGui::Text("Temperature");
			ImGui::SliderInt("Octaves##Temperature", &world.temperature_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Temperature", &world.temperature_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Temperature", &world.temperature_settings.amplitude, 0.0001f, 2.f);

			ImGui::Text("Moisture");
			ImGui::SliderInt("Octaves##Moisture", &world.moisture_settings.octaves, 1, 10);
			ImGui::SliderFloat("Frequency##Moisture", &world.moisture_settings.frequency, 0.0001f, 2.f);
			ImGui::SliderFloat("Amplitude##Moisture", &world.moisture_settings.amplitude, 0.0001f, 2.f);
		}
	}

	ImGui::End();
}
