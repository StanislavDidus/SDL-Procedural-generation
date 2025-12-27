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

static float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((x - inMin) / (inMax - inMin)) * (outMax - outMin);
}


Game::Game(Renderer& screen)
	: screen(screen)
	//, background(screen, Surface{ "assets/Sprites/bg1.png" }, {320.f, 180.f})
	//, player(screen, Surface{ "assets/Sprites/player.png" }, { 16.f, 32.f }, SDL_SCALEMODE_NEAREST)
	, tileset(screen, Surface{ "assets/Sprites/tileset.png" }, { 16.f, 16.f }, SDL_SCALEMODE_NEAREST)
	, world(screen)
	//, tilemap(world, tileset, 960.f, 540.f, 75.f, 100.f)
	, physics_system(component_manager, entity_manager)
	, input_system(component_manager, entity_manager)
	, collision_system(component_manager, entity_manager)
	, jump_system(component_manager, entity_manager)
	, mining_system(component_manager, entity_manager, world, 20.f, 20.f)
	, place_system(component_manager, entity_manager, world, 20.f, 20.f)
	, tilemap(world, tileset, collision_system, 20.f, 20.f)
	//, mapRange(0.f, 1.f, 0.f, 0.f)
{
	std::cout << "Game was created" << std::endl;

	screen.setView({ 0.f, 0.f });

	/*float min = 100.f;
	float max = -100.f;
	float mean = 0.f;

	float f = 0.03f;
	float a = 1.3f;

	for (int i = 0; i < 100'000; i++)
	{
		float noise_value = Noise::fractal1D<ValueNoise>(5, i, f, a, 1);

		if (noise_value < min) min = noise_value;
		if (noise_value > max) max = noise_value;
		mean += noise_value;
	}
	mean /= 100'000.f;

	std::cout << "ValueNoise: " << std::endl;
	std::cout << "Min: " << min << std::endl
		<< "Max: " << max << std::endl
		<< "Mean: " << mean << std::endl;*/

	//min = 100.f;
	//max = -100.f;
	//mean = 0.f;

	//for (int i = 0; i < 100'000; i++)
	//{
	//	float noise_value = Noise::fractal2D<PerlynNoise>(8, i, 0.f, f, a, 1);
	//	/*float centre = noise_value - 0.5f;
	//	float contrasted = tanh(8.f * centre);
	//	noise_value = (contrasted + 1.f) / 2.f;*/
	//	//noise_value = std::pow(noise_value, .5f);

	//	if (noise_value < min) min = noise_value;
	//	if (noise_value > max) max = noise_value;
	//	mean += noise_value;
	//}
	//mean /= 100'000.f;

	//std::cout << "PerlynNoise: " << std::endl;
	//std::cout << "Min: " << min << std::endl
	//	<< "Max: " << max << std::endl
	//	<< "Mean: " << mean << std::endl;

	/*mapRange.addPoint(0.25f, 1.f);
	mapRange.addPoint(0.5f, 0.f);
	mapRange.addPoint(0.75f, 1.f);*/

	/*std::cout << "0.f - " << mapRange.getValue(0.f) << std::endl;
	std::cout << "0.1f - " << mapRange.getValue(0.1f) << std::endl;
	std::cout << "0.25f - " << mapRange.getValue(0.25f) << std::endl;*/
	//std::cout << "0.37f - " << mapRange.getValue(0.37f) << std::endl;
	/*std::cout << "0.5f - " << mapRange.getValue(0.5f) << std::endl;
	std::cout << "0.62f - " << mapRange.getValue(0.62f) << std::endl;
	std::cout << "0.75f - " << mapRange.getValue(0.75f) << std::endl;
	std::cout << "0.87f - " << mapRange.getValue(0.87f) << std::endl;
	std::cout << "1.f - " << mapRange.getValue(1.f) << std::endl;
	std::cout << "-0.5.f - " << mapRange.getValue(-0.5f) << std::endl;
	std::cout << "1.5f - " << mapRange.getValue(1.f) << std::endl;*/

	/*EntityManager em;
	std::cout << em.createEntity().value_or(0) << std::endl;
	std::cout << em.createEntity().value_or(0) << std::endl;
	std::cout << em.createEntity().value_or(0) << std::endl;
	em.destroyEntity(9998);
	std::cout << em.createEntity().value_or(0) << std::endl;*/

	
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
		400.f,
		false
	};

	component_manager.player[player] = Player{

	};

	component_manager.mine_ability[player] = MineAbility
	{
		150.f,
		200.f
	};

	component_manager.mine_intent[player] = MineIntent
	{
		false
	};

	component_manager.place_ability[player] = PlaceAbility
	{
		200.f
	};

	component_manager.place_intent[player] = PlaceIntent
	{
		false
	};
}

Game::~Game()
{
	std::cout << "Game was deleted" << std::endl;
}

void Game::update(float dt)
{
	//Update
	updateInput(dt);

	screen.setZoom(zoom);
	screen.setView(view_position);

	

	//Check mouse state
	const auto& mouse = InputManager::getMouseState();
	/*if (mouse.left)
	{
		std::cout << "Left: " << mouse.position.x << ", " << mouse.position.y << std::endl;
	}
	if (mouse.right)
	{
		std::cout << "Right: " << mouse.position.x << ", " << mouse.position.y << std::endl;
	}*/

	input_system.update(dt);
	jump_system.update(dt);
	physics_system.update(dt);
	collision_system.update(dt);
	mining_system.update(dt, mouse, screen);
	place_system.update(dt, mouse, screen);

	updateTilemapTarget();

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// … your UI code
	updateImGui(dt);
	
	//Render
	tilemap.render(screen);

	auto& pos = component_manager.transform[player].position;
	auto& size = component_manager.transform[player].size;
	screen.drawRectangle(pos.x, pos.y, size.x, size.y, RenderType::FILL, Color::RED);

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), screen.getRenderer());

	//zoom = 1.f;


	//const auto& window_size = screen.getWindowSize();

	//float f = 0.02f;
	//float a = 1.f;

	//float min = 100.f;
	//float max = -100.f;

	//for (int x = 0; x < 480; x++)
	//{
	//	for (int y = 0; y < 480; y++)
	//	{
	//		float noise_value = Noise::fractal2D<PerlynNoise>(8, x, y, f, a, 1);
	//		/*float centre = noise_value - 0.5f;
	//		float contrasted = tanh(8.f * centre);
	//		noise_value = (contrasted + 1.f) / 2.f;*/
	//		//noise_value = std::pow(noise_value, 0.5f);

	//		if (noise_value < min) min = noise_value;
	//		if (noise_value > max) max = noise_value;
	//	}
	//}

	//for (int x = 0; x < 480; x++)
	//{
	//	for (int y = 0; y < 480; y++)
	//	{
	//		float noise_value = Noise::fractal2D<PerlynNoise>(8, x, y, f, a, 1);
	//		/*float centre = noise_value - 0.5f;
	//		float contrasted = tanh(8.f * centre);
	//		noise_value = (contrasted + 1.f) / 2.f;*/
	//		//noise_value = std::pow(noise_value, 0.5f);

	//		noise_value = (noise_value - min) / (max - min);
	//		
	//		uint8_t value = static_cast<uint8_t>(mapRange(noise_value, 0.f, 1.f, 0.f, 255.f));

	//		//assert(value >= 0 && value <= 255);

	//		//value = std::clamp(static_cast<int>(value), 0, 255);
	//		Color color{ value, value, value, 255 };
	//		screen.drawPoint(x, y, color);
	//	}
	//}

	/*for (int x = 480; x < 960; x++)
	{
		for (int y = 0; y < 480; y++)
		{
			float noise_value = Noise::fractal2D<ValueNoise>(8, x, y, 0.02f, 1.f, 1);

			uint8_t value = static_cast<uint8_t>(mapRange(noise_value, 0.f, 1.f, 0.f, 255.f));
			Color color{ value, value, value, 255 };
			screen.drawPoint(x, y, color);
		}
	}*/
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
		ImGui::SliderFloat("zoom", &zoom, 0.05f, 5.f);

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
		const char* items[] = { "Default", "PV", "Temperature", "Moisture" };

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
