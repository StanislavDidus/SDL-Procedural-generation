#include "Game.hpp"
#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"

#include "InputManager.hpp"
#include "ResourceManager.hpp"
#include "BaseState.hpp"

#include <random>

#include <algorithm>

#include "EnemyManager.hpp"
#include "tinyxml2.h"

#include "ECS/DeathSystem.hpp"
#include "RenderFunctions.hpp"
#include "ECS/ChangeMiningSizeSystem.hpp"
#include "UI/DynamicBackground.hpp"

using namespace graphics;

namespace
{
    std::random_device rd;
    std::minstd_rand rng(rd());
}

Game::Game(graphics::GpuRenderer& screen)
    : screen(screen)
{
    std::cout << "Game was created" << std::endl;
    
    ItemManager::get().loadXml(registry, "data/items.xml");
    TileManager::get().loadXml("data/tiles.xml");
    CraftingManager::get().loadXml(registry, "data/crafts.xml");
    ObjectManager::get().loadXml("data/objects.xml");
    EnemyManager::get().loadXml(registry, "data/enemies.xml");

    setState(GameState::MENU);

}

Game::~Game()
{
    std::cout << "Game was deleted" << std::endl;
}

void Game::initSystems()
{
    physics_system = std::make_unique<PhysicsSystem>(registry);
    input_system = std::make_unique<InputSystem>(registry);
    collision_system = std::make_shared<TileCollisionSystem>(registry, world->grid);
    jump_system = std::make_unique<JumpSystem>(registry);
    mining_tiles_system = std::make_unique<MiningTilesSystem>(registry, world, 20.f, 20.f);
    mining_objects_system = std::make_unique<MiningObjectsSystem>(registry, world, 20.f, 20.f);
    place_system = std::make_unique<PlaceSystem>(registry, world, 20.f, 20.f);
    item_usage_system = std::make_shared<ItemUsageSystem>(registry);
    craft_system = std::make_unique<CraftSystem>(registry);
    render_crafting_ui_system = std::make_unique<RenderCraftingUISystem>(registry, ui_settings);
    item_description_system = std::make_shared<ItemDescriptionSystem>(registry, ResourceManager::get().getFont("Main"),
                                                                      inventory_view, ui_settings);
    render_weapon_menu_system = std::make_unique<RenderWeaponMenuSystem>(registry, ui_settings);
    enemy_ai_system = std::make_unique<EnemyAISystem>(registry);
    enemy_spawn_system = std::make_shared<EnemySpawnSystem>(registry, world->grid, SpawnRadius{1500.0f, 2000.0f});
    player_combo_system = std::make_unique<PlayerComboSystem>(registry, enemy_spawn_system);
    apply_damage_system = std::make_unique<ApplyDamageSystem>(registry);
    display_hit_marks_system = std::make_unique<DisplayHitMarksSystem>(registry);
    drop_item_system = std::make_unique<DropItemSystem>(registry);
    inventory_manage_system = std::make_unique<InventoryManageSystem>(registry);
    apply_armor_effects = std::make_unique<ApplyArmorEffects>(registry);
    render_health_bar_system = std::make_unique<RenderHealthBarSystem>(registry);
    player_collision_system = std::make_unique<PlayerCollisionSystem>(registry);
    manage_invincible_status_system = std::make_unique<ManageInvincibleStatusSystem>(registry);
    death_system = std::make_unique<DeathSystem>(registry);
    change_mining_size_system = std::make_unique<ChangeMiningSizeSystem>(registry);
    chest_window_system = std::make_unique<ChestWindowSystem>(registry, ResourceManager::get().getFont("Main"));
    render_essence_counter = std::make_unique<RenderEssenceCounter>(registry, ui_settings, screen,
                                                                    ResourceManager::get().getFont("Main"));
    collect_essence_system = std::make_unique<CollectEssenceSystem>(registry);
    open_chest_system = std::make_unique<OpenChestSystem>(registry);
    drop_chest_loot_system = std::make_unique<DropChestLootSystem>(registry);
    health_regeneration_system = std::make_unique<HealthRegenerationSystem>(registry);
    update_effects_system = std::make_unique<UpdateEffects>(registry);
    apply_effects_system = std::make_unique<ApplyEffects>(registry);
    render_accessories_system = std::make_unique<RenderAccessoriesSystem>(
        registry, ui_settings, item_description_system);
    enemy_spawn_manager = std::make_unique<EnemySpawnManager>(enemy_spawn_system);
    fall_damage_system = std::make_unique<FallDamageSystem>(registry);
}

void Game::initGenerationData()
{
    generation_data.chest_size = glm::ivec2{2, 2};
    generation_data.y_base = 25.f;
    generation_data.cave_y_base = 30.0f;
    generation_data.sea_y_base = 19.f;
    generation_data.scale = 0.5f;
    generation_data.density_threshold = 0.5f;
    generation_data.chest_threshold = 0.05f;
    generation_data.drunk_walker_threshold = 0.2f;
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
        settings.frequency = 0.15f;
        settings.amplitude = 1.2f;
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
        settings.frequency = 0.1f;
        settings.amplitude = 1.0f;
    }

    //Moisture
    {
        NoiseSettings& settings = generation_data.noise_settings[NoiseType::MOISTURE];
        settings.octaves = 2;
        settings.frequency = 0.05f;
        settings.amplitude = 1.2f;
    }

    //Chest
    {
        NoiseSettings& settings = generation_data.noise_settings[NoiseType::CHEST];
        settings.octaves = 2;
        settings.frequency = 0.7f;
        settings.amplitude = 2.0f;
    }

    //Loot
    {
        NoiseSettings& settings = generation_data.noise_settings[NoiseType::LOOT];
        settings.octaves = 1;
        settings.frequency = 0.1f;
        settings.amplitude = 1.0f;
    }

    //Drunk Walker
    {
        NoiseSettings& settings = generation_data.noise_settings[NoiseType::DRUNK_WALKER];
        settings.octaves = 2;
        settings.frequency = 0.85f;
        settings.amplitude = 1.2f;
    }

    //Drunk Walker Movement
    {
        NoiseSettings& settings = generation_data.noise_settings[NoiseType::DRUNK_WALKER_MOVEMENT];
        settings.octaves = 3;
        settings.frequency = 1.1f;
        settings.amplitude = 1.2f;
    }

    //Objects
    {
        NoiseSettings& settings = generation_data.noise_settings[NoiseType::OBJECTS];
        settings.octaves = 2;
        settings.frequency = 0.5f;
        settings.amplitude = 1.0f;
    }
}

void Game::initMapRanges()
{
    //PV_HEIGHT
    {
        MapRange map{0.4f, 1.8f, 0.f, -25.f};
        generation_data.maps[MapRangeType::PV_HEIGHT] = map;
    }

    //PV_CHANGE
    {
        MapRange map{0.4f, 1.8f, 0.9f, 0.15f};
        generation_data.maps[MapRangeType::PV_CHANGE] = map;
    }

    //CAVE_HEIGHT
    {
        MapRange map{generation_data.cave_y_base, generation_data.cave_y_base + 300.f, 0.15f, 0.5f};

        map.addPoint(generation_data.cave_y_base + 25.f, 0.25f);

        generation_data.maps[MapRangeType::CAVE_CHANGE] = map;
    }

    //CAVE_SIZE_CHANGE
    {
        MapRange map{generation_data.cave_y_base, generation_data.cave_y_base + 150.0f, 400.0f, 1500.0f};
        map.addPoint(generation_data.cave_y_base + 30.0f, 950.0f);

        generation_data.maps[MapRangeType::CAVE_SIZE_CHANGE] = map;
    }
}

void Game::readItemNode(tinyxml2::XMLElement* item_node, LootType loot_type)
{
    const char* name = item_node->Attribute("ref");
    size_t id = ItemManager::get().getItemID(name);
    float weight = item_node->FloatAttribute("weight");
    int min_quantity = item_node->IntAttribute("min_quantity");
    int max_quantity = item_node->IntAttribute("max_quantity");
    generation_data.chest_loot[loot_type].emplace_back(id, weight, min_quantity, max_quantity);
}

void Game::initChestLoot()
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile("data/chestLoot.xml");

    const auto& loot_listing_node = doc.FirstChildElement("lootListing");

    const auto& base_loot_node = loot_listing_node->FirstChildElement("baseLoot");
    for (auto* item_node = base_loot_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->
         NextSiblingElement())
    {
        readItemNode(item_node, LootType::BASE);
    }

    const auto& common_loot_node = loot_listing_node->FirstChildElement("commonLoot");
    for (auto* item_node = common_loot_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->
         NextSiblingElement())
    {
        readItemNode(item_node, LootType::COMMON);
    }

    const auto& snow_loot_node = loot_listing_node->FirstChildElement("snowLoot");
    for (auto* item_node = snow_loot_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->
         NextSiblingElement())
    {
        readItemNode(item_node, LootType::SNOW);
    }

    const auto& sand_loot_node = loot_listing_node->FirstChildElement("sandLoot");
    for (auto* item_node = sand_loot_node->FirstChildElement("item"); item_node != nullptr; item_node = item_node->
         NextSiblingElement())
    {
        readItemNode(item_node, LootType::SAND);
    }
}

void Game::initBiomes()
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile("data/biomes.xml");

    const auto& biome_listing_node = doc.FirstChildElement("biomeListing");

    for (auto* biome_node = biome_listing_node->FirstChildElement("biome"); biome_node != nullptr; biome_node =
         biome_node->NextSiblingElement())
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

        generation_data.biomes.emplace_back(biome_name, pv_min, pv_max, temperature_min, temperature_max, moisture_min,
                                            moisture_max, surface_tile_id, dirt_tile_id);
    }
}

void Game::initPlayer()
{
    const auto& window_size = screen.getStandardWindowSize();
    player = registry.create();

    auto& base = registry.emplace<Components::BaseValues>(player);
    base.size = {35.0f, 40.0f};
    base.acceleration = {1500.0f, 0.0f};
    base.max_velocity = {250.0f, 200.0f};
    base.gravity = 1400.0f;

    auto& ts = registry.emplace<Components::Transform>(player);
    ts.position = glm::vec2{400.0f, -500.f};
    ts.position.x += 1000.0f * 20.0f;
    ts.size = glm::vec2{35.0f, 40.0f};
    base.size = ts.size;

    auto& renderable = registry.emplace<Components::Renderable>(player);
    renderable.sprite = (*ResourceManager::get().getSpriteSheet("player"))[0];
    renderable.priority = 1;

    registry.emplace<Components::AlwaysRender>(player);

    auto& physics = registry.emplace<Components::Physics>(player);
    physics.can_move_horizontal = true;
    physics.velocity = glm::vec2{0.0f};
    physics.acceleration = glm::vec2{1500.0f, 0.0f};
    physics.base_acceleration = physics.acceleration;

    physics.max_velocity = glm::vec2{250.0f, 200.0f};
    physics.decelaration = 5.0f;
    physics.gravity = 1400.0f;
    base.gravity = physics.gravity;

    auto& physics_step = registry.emplace<Components::PhysicStep>(player);
    physics_step.max_step_height = 20.0f;

    auto& jump = registry.emplace<Components::Jump>(player);
    jump.jump_force = 475.0f;

    registry.emplace<Components::Player>(player);

    auto& mine_intent = registry.emplace<Components::MineIntent>(player);

    auto& mining_ability = registry.emplace<Components::MiningAbility>(player);
    mining_ability.speed = BASE_MINING_SPEED;
    mining_ability.radius = BASE_MINING_RADIUS;
    mining_ability.max_size = BASE_MINING_SIZE;
    mining_ability.current_size = BASE_MINING_SIZE;

    auto& place_ability = registry.emplace<Components::PlaceAbility>(player);
    place_ability.radius = 200.0f;
    place_ability.placing_time = 0.3f;
    place_ability.placing_timer = 0.0f;

    auto& place_intent = registry.emplace<Components::PlaceIntent>(player);

    auto& has_inventory = registry.emplace<Components::HasInventory>(player);
    has_inventory.inventory = std::make_shared<Inventory>(registry, 15);

    auto& health = registry.emplace<Components::Health>(player);
    health.max_health = 100.0f;
    health.current_health = 100.0f;
    base.max_health = health.max_health;

    auto& regeneration = registry.emplace<Components::Regeneration>(player);
    regeneration.speed = 0.5f;

    registry.emplace<Components::CraftingAbility>(player);

    registry.emplace<Components::Equipment>(player, 2, 10);
    base.max_weapons = 2;
    registry.emplace<Components::EquipmentEssence>(player);

    auto& animation = registry.emplace<Components::CharacterAnimation>(player);
    animation.idle_animation = idle_animation;
    animation.running_animation = running_animation;
    animation.jump_animation = jump_animation;
    animation.fall_animation = fall_animation;

    auto& health_bar = registry.emplace<Components::HealthBar>(player);
    health_bar.position = glm::vec2{0.0f, static_cast<float>(window_size.y) - 70.0f};
    health_bar.size = glm::vec2{250.0f, 70.0f};
    health_bar.color = graphics::Color::RED;
    
    auto& fall_damage = registry.emplace<Components::FallDamage>(player);
    fall_damage.fall_damage_threshold = 200.0f;
    fall_damage.damage_per_height = 0.1f;

    //Add player a double jump
    //registry.emplace<Components::Effects::DoubleJump>(player);

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

void Game::initPlayerAnimations()
{
    idle_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 5.0f,
                                                       std::vector<int>{0, 1, 2});
    running_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 5.0f,
                                                          std::vector<int>{3, 4, 5, 6});
    jump_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 20.0f,
                                                       std::vector<int>{7});
    fall_animation = std::make_shared<SpriteAnimation>(ResourceManager::get().getSpriteSheet("player"), 20.0f,
                                                       std::vector<int>{8});
}

void Game::initUserInterface()
{
    if (registry.all_of<Components::Health>(player))
    {
        auto player_health = registry.get<Components::Health>(player);
    }

    //interface.addInventoryView(ResourceManager::get().getFont("Main"), ResourceManager::get().getSpriteSheet("items"), component_manager.has_inventory[player].inventory.get(), 3, 5, 50.f, {0.f, 0.f});

    inventory_view = std::make_shared<InventoryView>(registry, ResourceManager::get().getFont("Main"),
                                                     (*ResourceManager::get().getSpriteSheet("items")), 3, 5,
                                                     glm::vec2{0.f, 0.f}, ui_settings, player);
}

void Game::tick(float dt)
{
    time += dt;
    
    //Update
    update(dt);

    //Render
    render(dt);
}

void Game::update(float dt)
{
    updateInput(dt);

    screen.setZoom(zoom);
    screen.setView(view_position);

    const auto& mouse = InputManager::getMouseState();

    switch (current_state)
    {
    case GameState::NONE:
        /*
        if (InputManager::isKeyDown(SDL_SCANCODE_SPACE))
        {
            setState(GameState::PLAY);
        }
        */
        break;
    case GameState::MENU:
        button_system->update(screen);
        button_sound_system->update();
        button_sprite_system->update();
        manage_button_actions_system->update(player, screen);
        break;
    case GameState::PLAY:
        {
            background.update(screen.getView());
            input_system->update(screen, dt);
            jump_system->update(dt);
            physics_system->update(dt);
            collision_system->update(dt);
            button_system->update(screen);
            button_sound_system->update();
            button_sprite_system->update();
            manage_button_actions_system->update(player, screen);
            craft_system->update(player);
            enemy_ai_system->update(dt, player);
            player_combo_system->update(dt);
            apply_damage_system->update(dt);
            display_hit_marks_system->update(dt);
            render_weapon_circle_system->update(dt);
            item_usage_system->update();
            drop_item_system->update(dt);
            inventory_manage_system->update();
            apply_armor_effects->update();
            player_collision_system->update(player);
            manage_invincible_status_system->update(dt);
            death_system->update(*this);
            change_mining_size_system->update();
            chest_window_system->update(player, screen);
            collect_essence_system->update(player);
            open_chest_system->update();
            health_regeneration_system->update(dt);
            update_effects_system->update(dt);
            apply_effects_system->update(dt);
            enemy_spawn_manager->update(dt);
            button_sound_system->update();
            fall_damage_system->update();
            updateTimer(dt);
            //updateTilesDurability(world->grid);
            //world_renderer->update();

            world->update(registry);
            world->setSpriteMap(*tilemap);


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

            //world_generator->update(screen, dt, world_target);
            //world_generator->updateTiles();

            inventory_view->update();

            updateTilemapTarget();

            render_system->update(dt);
        }
        break;
    case GameState::PLAYER_DEAD:
        {
            physics_system->update(dt);
            collision_system->update(dt);
            display_hit_marks_system->update(dt);
        }
        break;
    }

    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    updateImGui(dt);
}

float menu_background_position_x1 = 0.0f;
float menu_background_position_x2 = 0.0f;
float menu_background_position_x3 = 0.0f;

void Game::render(float dt) const
{
    const auto& window_size = screen.getStandardWindowSize();
    const auto& view_position = screen.getView();

    switch (current_state)
    {
    case GameState::NONE:
        break;
    case GameState::MENU:
        {
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg2"), menu_background_position_x1, 0.0f, 960.0f, 540.0f);
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg2"), menu_background_position_x1 + 960.0f, 0.0f, 960.0f, 540.0f);
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg2"), menu_background_position_x1 - 960.0f, 0.0f, 960.0f, 540.0f);
            menu_background_position_x1 += 25.0f * dt;
            if (menu_background_position_x1 >= 960.0f)
                menu_background_position_x1 -= 960.0f;
            //
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg3"), menu_background_position_x2, 0.0f, 960.0f, 540.0f);
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg3"), menu_background_position_x2 + 960.0f, 0.0f, 960.0f, 540.0f);
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg3"), menu_background_position_x2 - 960.0f, 0.0f, 960.0f, 540.0f);
            menu_background_position_x2 += 50.0f * dt;
            if (menu_background_position_x2 >= 960.0f)
                menu_background_position_x2 -= 960.0f;
            //
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg4"), menu_background_position_x3, 0.0f, 960.0f, 540.0f);
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg4"), menu_background_position_x3 + 960.0f, 0.0f, 960.0f, 540.0f);
            graphics::drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("bg4"), menu_background_position_x3 - 960.0f, 0.0f, 960.0f, 540.0f);
            menu_background_position_x3 += 75.0f * dt;
            if (menu_background_position_x3 >= 960.0f)
                menu_background_position_x3 -= 960.0f;
        
            render_system->render(screen);
        
            float start_button_text_offset = 0.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("START1"),
                ui_settings.menu_start_button_position.x + start_button_text_offset,
                ui_settings.menu_start_button_position.y + 10.f + std::sin(time * 2.0f) * ui_settings.letter_bouncing * 1.0f,
                50.0f,
                80.0f);
            start_button_text_offset += 55.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("START2"),
                ui_settings.menu_start_button_position.x + start_button_text_offset,
                ui_settings.menu_start_button_position.y + 10.f + std::sin(time * 2.0f) * ui_settings.letter_bouncing * 0.6f,
                50.0f,
                80.0f);
            start_button_text_offset += 55.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("START3"),
                ui_settings.menu_start_button_position.x + start_button_text_offset,
                ui_settings.menu_start_button_position.y + 10.f + std::sin(time * 2.0f) * ui_settings.letter_bouncing * 0.25f,
                50.0f,
                80.0f);
            start_button_text_offset += 55.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("START4"),
                ui_settings.menu_start_button_position.x + start_button_text_offset,
                ui_settings.menu_start_button_position.y + 10.f + std::sin(time * 2.0f) * ui_settings.letter_bouncing * 0.1f,
                50.0f,
                80.0f);
            start_button_text_offset += 55.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("START5"),
                ui_settings.menu_start_button_position.x + start_button_text_offset,
                ui_settings.menu_start_button_position.y + 10.f + std::sin(time * 2.0f) * ui_settings.letter_bouncing * 0.6f,
                50.0f,
                80.0f);
            
            float exit_button_text_offset = 20.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT1"),
                ui_settings.menu_exit_button_position.x + exit_button_text_offset,
                ui_settings.menu_exit_button_position.y + 15.f + std::sin(time) * ui_settings.letter_bouncing * 0.8f,
                48.0f,
                75.0f);
            exit_button_text_offset += 51.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT2"),
                ui_settings.menu_exit_button_position.x + exit_button_text_offset,
                ui_settings.menu_exit_button_position.y + 15.f + std::sin(time) * ui_settings.letter_bouncing * 0.3f,
                48.0f,
                75.0f);
            exit_button_text_offset += 51.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT3"),
                ui_settings.menu_exit_button_position.x + exit_button_text_offset,
                ui_settings.menu_exit_button_position.y + 15.f + std::sin(time) * ui_settings.letter_bouncing * 0.1f,
                43.0f,
                70.0f);
            exit_button_text_offset += 50.0f;
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT4"),
                ui_settings.menu_exit_button_position.x + exit_button_text_offset,
                ui_settings.menu_exit_button_position.y + 15.f + std::sin(time) * ui_settings.letter_bouncing * 0.7f,
                55.0f,
                75.0f);
            
            drawScaledSprite(screen, ResourceManager::get().getSpriteSheet("menu")->getSprite("LOGO"),
                ui_settings.logo_position.x, ui_settings.logo_position.y,
                ui_settings.logo_size.x, ui_settings.logo_size.y);
        }
        break;
    case GameState::PLAY:
    case GameState::PLAYER_DEAD:
        {
            //Draw Sky
            //auto sky_sprite = ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("Sky");
            //graphics::drawScaledSprite(screen, sky_sprite, 0.0f, 0.0f, static_cast<float>(window_size.x), static_cast<float>(window_size.y), graphics::IGNORE_VIEW_ZOOM);

            //world_renderer->render(screen, world_target);
            //world_generator->render(screen);
            background.render(screen, screen.getView());
            screen.renderTileMap(tilemap, 0.0f, 0.0f);
            render_system->render(screen);
            mining_tiles_system->renderOutline(screen);
            mining_objects_system->render(screen);
            render_weapon_circle_system->render(screen);
            drop_chest_loot_system->update(dt, screen);

            //UI
            inventory_view->render(screen);
            render_essence_counter->render(screen, player);
            render_crafting_ui_system->render(screen, player);
            item_description_system->render(screen, player);
            render_weapon_menu_system->render(screen, player);
            render_health_bar_system->render(screen);
            chest_window_system->render(screen);
            render_accessories_system->update(player, screen);
        }
        break;
    }

    //Set window base size to properly render imgui without scaling
    /*SDL_SetRenderLogicalPresentation(screen.getDevice(), 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), screen.getDevice());
    SDL_SetRenderLogicalPresentation(screen.getDevice(), window_size.x, window_size.y, SDL_LOGICAL_PRESENTATION_LETTERBOX);*/
}

void Game::setState(GameState new_state)
{
    if (new_state == current_state) return;

    exitState(current_state);

    current_state = new_state;

    enterState(current_state);
}

void Game::enterState(GameState state)
{
    switch (state)
    {
    case GameState::NONE:
        {
           
        }
        break;
    case GameState::MENU:
        {
            button_system = std::make_unique<ButtonSystem>(registry);
            manage_button_actions_system = std::make_unique<ManageButtonActionsSystem>(registry);
            render_system = std::make_unique<RenderSystem>(registry);
            button_sound_system = std::make_unique<ButtonSoundSystem>(registry);
            button_sprite_system = std::make_unique<ButtonSpriteSystem>(registry);
            
            ResourceManager::get().getSound("Menu Music")->play();

            {
                Entity start_button = registry.create();
                auto& ts = registry.emplace<Components::Transform>(start_button);
                ts.position = ui_settings.menu_start_button_position;
                ts.size = ui_settings.menu_start_button_size;
                auto& button = registry.emplace<Components::Button>(start_button);
                auto& func = registry.emplace<Components::ButtonFunction>(start_button);
                func.command = [&]{setState(GameState::PLAY); };
                auto& render = registry.emplace<Components::Renderable>(start_button);
                render.sprite = ResourceManager::get().getSpriteSheet("menu")->getSprite("START_MENU");
                registry.emplace<Components::AlwaysRender>(start_button);
                registry.emplace<Components::ButtonReleasedSound>(start_button, ResourceManager::get().getSound("Button Released"));
                registry.emplace<Components::ButtonEnteredSound>(start_button, ResourceManager::get().getSound("Button Entered"));
                registry.emplace<Components::ButtonEnteredSprite>(start_button, ResourceManager::get().getSpriteSheet("menu")->getSprite("START_MENU_ENTER"));
                registry.emplace<Components::ButtonExitSprite>(start_button, ResourceManager::get().getSpriteSheet("menu")->getSprite("START_MENU"));
            }
            {
                Entity exit_button = registry.create();
                auto& ts = registry.emplace<Components::Transform>(exit_button);
                ts.position = ui_settings.menu_exit_button_position;
                ts.size = ui_settings.menu_exit_button_size;
                auto& button = registry.emplace<Components::Button>(exit_button);
                auto& func = registry.emplace<Components::ButtonFunction>(exit_button);
                func.command = [&]{std::exit(0); };
                auto& render = registry.emplace<Components::Renderable>(exit_button);
                render.sprite = ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT_MENU");
                registry.emplace<Components::AlwaysRender>(exit_button);
                registry.emplace<Components::ButtonReleasedSound>(exit_button, ResourceManager::get().getSound("Button Released"));
                registry.emplace<Components::ButtonEnteredSound>(exit_button, ResourceManager::get().getSound("Button Entered"));
                registry.emplace<Components::ButtonEnteredSprite>(exit_button, ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT_MENU_ENTER"));
                registry.emplace<Components::ButtonExitSprite>(exit_button, ResourceManager::get().getSpriteSheet("menu")->getSprite("EXIT_MENU"));
            }
        }
        break;
    case GameState::PLAY:
        {
           // Destroy all menu buttons
            std::vector<Entity> buttons;
            auto view = registry.view<Components::Button>();
            for (auto [entity, button] : view.each())
            {
                buttons.push_back(entity);
            }
            for (const auto& button : buttons)
            {
                registry.destroy(button);
            }
            
            initNoiseSettings();
            initMapRanges();
            initGenerationData();
            initBiomes();
            initChestLoot();

            text = std::make_unique<Text>(screen, ResourceManager::get().getFont("Main"), "Player");
            world_generator = std::make_unique<WorldGenerator>(generation_data, registry, 2000, 350);
            world = world_generator->generateWorld(0);
            world->initWorld(registry, 20.0f, 20.0f);
            //spawnObjects(registry, *world, 20.0f, 20.0f);

            initUserInterface();
            initSystems();

            //Set player idle animation
            initPlayerAnimations();

            initPlayer();
            item_usage_system = std::make_shared<ItemUsageSystem>(registry);
            render_weapon_circle_system = std::make_unique<RenderWeaponCircle>(registry, player);
            craft_view = std::make_unique<CraftView>(registry, player, 5, 5, 60.f, glm::vec2{660.f, 0.f});
            inventory_view->setTargetEntity(player);

            //world_renderer->spawnObjects();

            setState(GameState::PLAY);

            //Give basic items to the player
            auto& inventory = registry.get<Components::HasInventory>(player).inventory;
            inventory->addItem(ItemManager::get().getItemID("Heal_Potion"), 10);
            inventory->addItem(ItemManager::get().getItemID("Wood"), 20);
            inventory->addItem(ItemManager::get().getItemID("Rope"), 3);
            inventory->addItem(ItemManager::get().getItemID("Common_Belt"), 1);
            inventory->addItem(ItemManager::get().getItemID("Common_Pickaxe"), 1);
            inventory->addItem(ItemManager::get().getItemID("Magic_Boots"), 1);
            inventory->addItem(ItemManager::get().getItemID("Common_Boots"), 1);
            inventory->addItem(ItemManager::get().getItemID("Big_Armor"), 1);
            inventory->addItem(ItemManager::get().getItemID("Fast_Helmet"), 1);
            inventory->addItem(ItemManager::get().getItemID("Snow_Bow"), 1);
            inventory->addItem(ItemManager::get().getItemID("Dagger_With_Poison"), 1);
            inventory->addItem(ItemManager::get().getItemID("Jumper"), 1);


            //Test accessories
            auto& accessories = registry.get<Components::Equipment>(player).accessories;
            registry.emplace<Components::EquipItem>(
                player, ItemManager::get().createItem(registry, ItemManager::get().getItemID("Astronaut_Suit"), 1));
            //registry.emplace_or_replace<Components::EquipItem>(player, ItemManager::get().createItem(registry, ItemManager::get().getItemID("Common_Belt"), 1));
            //accessories.push_back(ItemManager::get().createItem(registry, ItemManager::get().getItemID("Big_Armor"), 1));

            auto tilemap_texture = ResourceManager::get().getSpriteSheet("tiles")->getTexture();
            tilemap = std::make_shared<TileMap>(screen.getDevice(), tilemap_texture, 2000, 350, 20, 20, 26, 25, 0.0f, 0.0f);

            ResourceManager::get().getSound("Background Music")->play();

            break;
        }
    case GameState::PLAYER_DEAD:
        break;
    }
}

void Game::exitState(GameState state)
{
    switch (state)
    {
    case GameState::NONE:
        break;
    case GameState::MENU:
        ResourceManager::get().getSound("Menu Music")->stop();
        break;
    case GameState::PLAY:
            break;
    case GameState::PLAYER_DEAD:
        break;
    }
}

void Game::updateTimer(float dt)
{
    {
        auto view = registry.view<Components::InventoryItems::JumpComponent>();
        for (auto [entity, jump_component] : view.each())
        {
            jump_component.timer += dt;
        }
    }
}

void Game::updateTilemapTarget()
{
    if (lock_camera)
    {
        //Set target on player
        const auto& player_transform = registry.get<Components::Transform>(player);
        const auto& player_position = player_transform.position;
        const auto& player_size = player_transform.size;
        world_target = {player_position.x + player_size.x / 2.f, player_position.y + player_size.y / 2.f};
        //glm::vec2 player_screen_position = world_target - static_cast<glm::vec2>(screen.getWindowSize()) / 2.f;
        view_position = world_target;
        view_position -= glm::vec2{screen.getStandardWindowSize().x * 0.5f, screen.getStandardWindowSize().y * 0.5f};
    }
    else
    {
        const auto& window_size = static_cast<glm::vec2>(screen.getStandardWindowSize());

        glm::vec2 view_center = {view_position.x + window_size.x / 2.f, view_position.y + window_size.y / 2.f};

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
}

void Game::updateImGui(float dt)
{
    switch (current_state)
    {
    case GameState::NONE:
        break;
    case GameState::MENU:
        break;
    case GameState::PLAY:
    case GameState::PLAYER_DEAD:
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
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

            if (ImGui::Button("Generate world_generator"))
            {
                world = world_generator->generateWorld(buffer);
                world->initWorld(registry, 20.0f, 20.0f);
            }

            ImGui::SameLine();

            if (ImGui::Button("Generate world_generator with random seed"))
            {
                std::uniform_int_distribution dist(0, 10000000);
                buffer = dist(rng);
                world = world_generator->generateWorld(buffer);
                world->initWorld(registry, 20.0f, 20.0f);
            }

            //ImGui::Checkbox("Use New Cave Generation Algorithm", &world_generator->use_new_cave_generation);
            //ImGui::Checkbox("Use Cellular Automata Algorithm", &world_generator->use_cellular_automata);


            //ImGui::SliderFloat("scale", &world_generator->scale, 0.f, 1.f);
            //ImGui::SliderFloat("density_change", &world_generator->density_change, 0.1f, 1.f);
            //ImGui::SliderFloat("y_base", &world_generator->y_base, -100.f, 100.f);
            //ImGui::SliderFloat("sea_level", &world_generator->sea_level, -100.f, 100.f);

            //ImGui::SliderFloat("cave_threshold_min", &world_generator->cave_threshold_min, 0.1f, 1.0f);
            //ImGui::SliderFloat("cave_threshold_max", &world_generator->cave_threshold_max, 0.1f, 1.0f);
            //ImGui::SliderFloat("cave_threshold_step", &world_generator->cave_threshold_step, 0.0001f, 0.1f);
            //ImGui::SliderFloat("cave_base_y", &world_generator->cave_base_height, -100.f, 100.f);

            static int current = 0;
            const char* items[] = {"Default", "PV", "Temperature", "Moisture", "Durability"};

            //ImGui::Combo("Render Mode", &tilemap->render_mode, items, IM_ARRAYSIZE(items));

            /*if (ImGui::CollapsingHeader("Terrain & Nature"))
            {
                ImGui::Text("PV");
                ImGui::SliderInt("Octaves##PV", &world_generator->peaks_and_valleys_settings.octaves, 1, 10);
                ImGui::SliderFloat("Frequency##PV", &world_generator->peaks_and_valleys_settings.frequency, 0.0001f, 2.f);
                ImGui::SliderFloat("Amplitude##PV", &world_generator->peaks_and_valleys_settings.amplitude, 0.0001f, 2.f);

                ImGui::Text("Density");
                ImGui::SliderInt("Octaves##Density", &world_generator->density_settings.octaves, 1, 10);
                ImGui::SliderFloat("Frequency##Density", &world_generator->density_settings.frequency, 0.0001f, 2.f);
                ImGui::SliderFloat("Amplitude##Density", &world_generator->density_settings.amplitude, 0.0001f, 2.f);

                ImGui::Text("Caves");
                ImGui::SliderInt("Octaves##Caves", &world_generator->cave_settings.octaves, 1, 10);
                ImGui::SliderFloat("Frequency##Caves", &world_generator->cave_settings.frequency, 0.0001f, 2.f);
                ImGui::SliderFloat("Amplitude##Caves", &world_generator->cave_settings.amplitude, 0.0001f, 2.f);

                ImGui::Text("Tunnels");
                ImGui::SliderInt("Octaves##Tunnels", &world_generator->tunnel_settings.octaves, 1, 10);
                ImGui::SliderFloat("Frequency##Tunnels", &world_generator->tunnel_settings.frequency, 0.001f, 2.f);
                ImGui::SliderFloat("Amplitude##Tunnels", &world_generator->tunnel_settings.amplitude, 0.001f, 2.f);

                ImGui::Text("Temperature");
                ImGui::SliderInt("Octaves##Temperature", &world_generator->temperature_settings.octaves, 1, 10);
                ImGui::SliderFloat("Frequency##Temperature", &world_generator->temperature_settings.frequency, 0.0001f, 2.f);
                ImGui::SliderFloat("Amplitude##Temperature", &world_generator->temperature_settings.amplitude, 0.0001f, 2.f);

                ImGui::Text("Moisture");
                ImGui::SliderInt("Octaves##Moisture", &world_generator->moisture_settings.octaves, 1, 10);
                ImGui::SliderFloat("Frequency##Moisture", &world_generator->moisture_settings.frequency, 0.0001f, 2.f);
                ImGui::SliderFloat("Amplitude##Moisture", &world_generator->moisture_settings.amplitude, 0.0001f, 2.f);
            }*/
        }
        ImGui::End();
        break;
    }
}
