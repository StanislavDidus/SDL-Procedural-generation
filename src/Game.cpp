#include "Game.hpp"
#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "InputManager.hpp"
#include "ResourceManager.hpp"

#include <random>

#include <algorithm>

#include "EnemyManager.hpp"
#include "tinyxml2.h"


#include "RenderFunctions.hpp"

using namespace graphics;

namespace
{
	std::random_device rd;
	std::minstd_rand rng(rd());
}

Game::Game(graphics::Renderer& screen)
	: screen(screen)
{
	std::cout << "Game was created" << std::endl;

	ResourceManager::get().loadXml("data/assets.xml", screen);
	ItemManager::get().loadXml("data/items.xml");
	TileManager::get().loadXml("data/tiles.xml");
	CraftingManager::get().loadXml("data/crafts.xml");
	ObjectManager::get().loadXml("data/objects.xml");
	EnemyManager::get().loadXml("data/enemies.xml");

	initNoiseSettings();
	initMapRanges();
	initGenerationData();
	initBiomes();

	text = std::make_unique<Text>(ResourceManager::get().getFont("Main"), screen, "Player");
	world = std::make_shared<World>(generation_data, collision_system, 500, 200, 20.f, 20.f);

	initUserInterface();
	initSystems();

	//Set player idle animation
	idle_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 5.0f, std::vector<int>{0, 1, 2});
	running_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 5.0f, std::vector<int>{3, 4, 5, 6});
	jump_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 20.0f, std::vector<int>{7});
	fall_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 20.0f, std::vector<int>{8});

	initPlayer();
	item_usage_system = std::make_shared<ItemUsageSystem>(player);
	render_weapon_circle_system = std::make_unique<RenderWeaponCircle>(player);
	craft_view = std::make_unique<CraftView>(player, 5, 5, 60.f, glm::vec2{ 660.f, 0.f });
	inventory_view->setTargetEntity(player);

	world->generateWorld(0);

	//Give basic items to the player
	auto& inventory = registry.get<Components::HasInventory>(player).inventory;
	inventory->addItem(ItemManager::get().getItemID("Apple"), 15);
	inventory->addItem(ItemManager::get().getItemID("Banana"), 1);
	inventory->addItem(ItemManager::get().getItemID("Heal_Potion"), 1);
	inventory->addItem(ItemManager::get().getItemID("Cactus"), 1);
	inventory->addItem(ItemManager::get().getItemID("Wood"), 20);
	inventory->addItem(ItemManager::get().getItemID("Rope"), 3);
	inventory->addItem(ItemManager::get().getItemID("Gold"), 10);
	inventory->addItem(ItemManager::get().getItemID("Common_Pickaxe"), 1);
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

void Game::initSystems()
{
	physics_system = std::make_unique<PhysicsSystem>();
	input_system = std::make_unique<InputSystem>();
	collision_system = std::make_shared<TileCollisionSystem>(world);
	jump_system = std::make_unique<JumpSystem>();
	mining_tiles_system = std::make_unique<MiningTilesSystem>(*world, 20.f, 20.f);
	mining_objects_system = std::make_unique<MiningObjectsSystem>(*world, 20.f, 20.f);
	place_system = std::make_unique<PlaceSystem>(*world, 20.f, 20.f);
	item_usage_system = std::make_shared<ItemUsageSystem>(player);
	button_system = std::make_unique<ButtonSystem>();
	craft_system = std::make_unique<CraftSystem>();
	render_crafting_ui_system = std::make_unique<RenderCraftingUISystem>(ui_settings);
	item_description_system = std::make_unique<ItemDescriptionSystem>(ResourceManager::get().getFont("Main"), inventory_view, ui_settings);
	render_system = std::make_unique<RenderSystem>();
	render_weapon_menu_system = std::make_unique<RenderWeaponMenuSystem>(ui_settings);
	enemy_ai_system = std::make_unique<EnemyAISystem>();
	enemy_spawn_system = std::make_shared<EnemySpawnSystem>(*world, SpawnRadius{ 1100.0f, 1400.0f });
	player_combo_system = std::make_unique<PlayerComboSystem>(enemy_spawn_system);
	apply_damage_system = std::make_unique<ApplyDamageSystem>();
	display_hit_marks_system = std::make_unique<DisplayHitMarksSystem>();
	drop_item_system = std::make_unique<DropItemSystem>();
	inventory_manage_system = std::make_unique<InventoryManageSystem>();

	world->setCollisionSystem(collision_system);
}

void Game::initGenerationData()
{
	generation_data.y_base = 25.f;
	generation_data.cave_y_base = 10.f;
	generation_data.sea_y_base = 19.f;
	generation_data.scale = 0.5f;
	generation_data.density_threshold = 0.5f;
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
	tinyxml2::XMLDocument doc;
	doc.LoadFile("data/biomes.xml");

	const auto& biome_listing_node = doc.FirstChildElement("biomeListing");

	for (auto* biome_node = biome_listing_node->FirstChildElement("biome"); biome_node != nullptr; biome_node = biome_node->NextSiblingElement())
	{
		std::string biome_name = biome_node->Attribute("id");

		const auto& pv_node = biome_node->FirstChildElement("pv");
		float pv_min;
		float pv_max;
		pv_node->QueryFloatAttribute("min", &pv_min);
		pv_node->QueryFloatAttribute("max", &pv_max);

		const auto& temperature_node = biome_node->FirstChildElement("temperature");
		float temperature_min;
		float temperature_max;
		temperature_node->QueryFloatAttribute("min", &temperature_min);
		temperature_node->QueryFloatAttribute("max", &temperature_max);

		const auto& moisture_node = biome_node->FirstChildElement("moisture");
		float moisture_min;
		float moisture_max;
		moisture_node->QueryFloatAttribute("min", &moisture_min);
		moisture_node->QueryFloatAttribute("max", &moisture_max);

		size_t surface_tile_id = TileManager::get().getTileID(biome_node->FirstChildElement("surfaceTile")->GetText());
		size_t dirt_tile_id = TileManager::get().getTileID(biome_node->FirstChildElement("dirtTile")->GetText());

		generation_data.biomes.emplace_back(biome_name, pv_min, pv_max, temperature_min, temperature_max, moisture_min, moisture_max, surface_tile_id, dirt_tile_id);
	}
}

void Game::initPlayer()
{

	player = registry.create();

	auto& ts = registry.emplace<Components::Transform>(player);
	ts.position = glm::vec2{ 400.0f, -500.f };
	ts.size = glm::vec2{ 35.0f, 40.0f };

	auto& renderable = registry.emplace<Components::Renderable>(player);
	renderable.sprite = (*ResourceManager::get().getSpriteSheet("player"))[0];

	auto& physics = registry.emplace<Components::Physics>(player);
	physics.can_move_horizontal = true;
	physics.velocity = glm::vec2{ 0.0f };
	physics.acceleration = glm::vec2{ 1500.0f, 0.0f };
	physics.acceleration = glm::vec2{ 250.0f, 200.0f };
	physics.decelaration = 5.0f;

	auto& physics_step = registry.emplace<Components::PhysicStep>(player);
	physics_step.max_step_height = 20.0f;

	auto& jump = registry.emplace<Components::Jump>(player);
	jump.jump_force = 475.0f;

	registry.emplace<Components::Player>(player);

	auto& mine_intent = registry.emplace<Components::MineIntent>(player);
	
	auto& mining_ability = registry.emplace<Components::MiningAbility>(player);
	mining_ability.speed = BASE_MINING_SPEED;
	mining_ability.radius = BASE_MINING_RADIUS;
	mining_ability.size = BASE_MINING_SIZE;

	auto& place_ability = registry.emplace<Components::PlaceAbility>(player);
	place_ability.radius = 200.0f;
	place_ability.placing_time = 0.3f;
	place_ability.placing_timer = 0.0f;

	auto& place_intent = registry.emplace<Components::PlaceIntent>(player);

	auto& has_inventory = registry.emplace<Components::HasInventory>(player);
	has_inventory.inventory = std::make_shared<Inventory>(15);

	auto& health = registry.emplace<Components::Health>(player);
	health.max_health = 100.0f;
	health.current_health = 50.0f;

	registry.emplace<Components::CraftingAbility>(player);

	registry.emplace<Components::Equipment>(player, 2);

	auto& animation = registry.emplace<Components::CharacterAnimation>(player);
	animation.idle_animation = idle_animation;
	animation.running_animation = running_animation;
	animation.jump_animation = jump_animation;
	animation.fall_animation = fall_animation;

	//Add all recipes that do not require blueprints
	for (size_t i = 0; i < CraftingManager::get().size(); ++i)
	{
		const auto& recipe = CraftingManager::get().getRecipe(i);

		if (!recipe.is_blueprint_required)
		{
			auto& craft_abl = registry.get<Components::CraftingAbility>(player);
			craft_abl.recipes_acquired.emplace_back(i);
		}
	}

}

void Game::initUserInterface()
{
	auto player_health = registry.get<Components::Health>(player);

	interface.addFillBar({ 0.f, screen.getWindowSize().y - 50.f, }, { 250.f, 50.f }, player_health.current_health, 100.f, Color::RED);

	//interface.addInventoryView(ResourceManager::get().getFont("Main"), ResourceManager::get().getSpriteSheet("items"), component_manager.has_inventory[player].inventory.get(), 3, 5, 50.f, {0.f, 0.f});

	inventory_view = std::make_shared<InventoryView>(ResourceManager::get().getFont("Main"), (*ResourceManager::get().getSpriteSheet("items")), 3, 5, glm::vec2{0.f, 0.f}, ui_settings, player);
}

void Game::update(float dt)
{
	//Update
	updateInput(dt);

	screen.setZoom(zoom);
	screen.setView(view_position);

	const auto& mouse = InputManager::getMouseState();

	input_system->update(screen, dt);
	jump_system->update(dt);
	physics_system->update(dt);
	collision_system->update(dt);
	button_system->update();
	craft_system->update(player);
	enemy_ai_system->update(dt, player);
	player_combo_system->update(dt, player);
	apply_damage_system->update(dt);
	display_hit_marks_system->update(dt);
	render_weapon_circle_system->update(dt);
	item_usage_system->update();
	drop_item_system->update(dt);
	inventory_manage_system->update();
	
	const auto& player_transform = registry.get<Components::Transform>(player);
	const auto& player_pos = player_transform.position;
	const auto& player_size = player_transform.size;
	enemy_spawn_system->update(dt, player_pos + player_size * 0.5f, screen);

	if (!inventory_view->isMouseCoveringInventory() || registry.all_of<Components::MineObjectsState>(player))
	{
		mining_tiles_system->update(dt);
		place_system->update(dt);
		mining_objects_system->update(dt);
	}

	world->update(screen, dt, world_target);
	world->updateTiles();

	inventory_view->update();

	updateTilemapTarget();

	render_system->update(dt);
	
	//Render
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	updateImGui(dt);

	const auto& window_size = screen.getWindowSize();

	//collision_system->collisions.clear();

	world->render(screen);

	render_system->render(screen);

	mining_tiles_system->renderOutline(screen);

	mining_objects_system->render(screen);

	render_weapon_circle_system->render(screen);

	//UI
	inventory_view->render(screen);
	render_crafting_ui_system->render(screen, player);
	item_description_system->render(screen, player);
	render_weapon_menu_system->render(screen, player);
	
	//Set window base size to properly render imgui without scaling
	SDL_SetRenderLogicalPresentation(screen.get(), 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), screen.get());

	SDL_SetRenderLogicalPresentation(screen.get(), window_size.x, window_size.y, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void Game::updateTilemapTarget()
{
	if (lock_camera)
	{
		//Set target on player
		const auto& player_transform = registry.get<Components::Transform>(player);
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

		world_target = view_position;
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
		registry.get<Components::HasInventory>(player).inventory->addItem(1, 1);
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
		/*if (component_manager.mine_tiles_ability.contains(player))
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
		}*/
		
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SliderFloat("camera_move_speed", &camera_move_speed, 1000.f, 10000.f);
		ImGui::SliderFloat("zoom", &zoom, 0.1f, 5.f);

		ImGui::Checkbox("Lock camera on player", &lock_camera);
	}

	if (ImGui::CollapsingHeader("UI"))
	{
		if (ImGui::SliderFloat("UI Scale", &ui_scale, 0.5f, 1.7f))
		{
			ui_settings = UISettings(ui_scale);
		}
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
