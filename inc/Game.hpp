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
#include "ECS/Components.hpp"
#include "ItemManager.hpp"
#include "ObjectManager.hpp"
#include "GenerationData.hpp"
#include <memory>

#include "CraftingManager.hpp"
#include "CraftView.hpp"
#include "Surface.hpp"
#include "ECS/ItemDescriptionSystem.hpp"
#include "ECS/RenderSystem.hpp"
#include "ECS/RenderCraftingUISystem.hpp"
#include "ECS/RenderWeaponMenuSystem.hpp"
#include "UI/UISettings.hpp"

#include "SpriteAnimation.hpp"
#include "EnemySpawnSystem.hpp"
#include "tinyxml2.h"
#include "ECS/ApplyDamageSystem.hpp"
#include "ECS/DisplayHitMarksSystem.hpp"
#include "ECS/DropItemSystem.hpp"
#include "ECS/EnemyAISystem.hpp"
#include "ECS/InventoryManageSystem.hpp"
#include "ECS/PlayerComboSystem.hpp"
#include "ECS/RenderWeaponCircle.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ChestWindowSystem.hpp"
#include "ECS/CollectEssenceSystem.hpp"
#include "ECS/ManageButtonActionsSystem.hpp"
#include "ECS/ManageInvincibleStatusSystem.hpp"
#include "ECS/OpenChestSystem.hpp"
#include "ECS/PlayerCollisionSystem.hpp"
#include "ECS/RenderEssenceCounter.hpp"
#include "ECS/RenderHealthBarSystem.hpp"

class ChangeMiningSizeSystem;
class DeathSystem;

enum class GameState
{
	NONE,
	PLAY,
	PLAYER_DEAD,
};

class Game
{
public:
	explicit Game(graphics::Renderer& screen);
	virtual ~Game();

	void tick(float dt);

	void setState(GameState new_state);
private:
	void initSystems();
	void initGenerationData();
	void initNoiseSettings();
	void initMapRanges();
	void readItemNode(tinyxml2::XMLElement* item_node, LootType loot_type);
	void initChestLoot();
	void initBiomes();
	void initPlayer();
	void initPlayerAnimations();
	void initUserInterface();

	void updateTilemapTarget();
	void updateInput(float dt);
	void updateImGui(float dt);

	void update(float dt);
	void render() const;

	void enterState(GameState state);
	void exitState(GameState state);

	GameState current_state = GameState::NONE;

	bool lock_camera = true;

	glm::vec2 view_position = { 0.f, 0.f };

	float min_zoom = 0.5f;
	float max_zoom = 2.f;
	float zoom = 1.f;

	float camera_move_speed = 1500.f;

	graphics::Renderer& screen;

	std::unique_ptr<TileMap> tilemap;

	glm::vec2 world_target;
	std::shared_ptr<World> world;

	float tilemap_rows = 10.f;
	float tilemap_columns = 10.f;

	entt::registry registry;

	//Systems
	std::unique_ptr<PhysicsSystem> physics_system;
	std::unique_ptr<InputSystem> input_system;
	std::shared_ptr<TileCollisionSystem> collision_system;
	std::unique_ptr<JumpSystem> jump_system;
	std::unique_ptr<MiningTilesSystem> mining_tiles_system;
	std::unique_ptr<MiningObjectsSystem> mining_objects_system;
	std::unique_ptr<PlaceSystem> place_system;
	std::shared_ptr<ItemUsageSystem> item_usage_system;
	std::unique_ptr<ButtonSystem> button_system;
	std::unique_ptr<CraftSystem> craft_system;
	std::unique_ptr<RenderCraftingUISystem> render_crafting_ui_system;
	std::unique_ptr<RenderWeaponMenuSystem> render_weapon_menu_system;
	std::unique_ptr<ItemDescriptionSystem> item_description_system;
	std::unique_ptr<RenderSystem> render_system;
	std::unique_ptr<EnemyAISystem> enemy_ai_system;
	std::unique_ptr<ApplyDamageSystem> apply_damage_system;
	std::unique_ptr<DisplayHitMarksSystem> display_hit_marks_system;
	std::unique_ptr<RenderWeaponCircle> render_weapon_circle_system;
	std::unique_ptr<DropItemSystem> drop_item_system;
	std::unique_ptr<InventoryManageSystem> inventory_manage_system;
	std::unique_ptr<ApplyArmorEffects> apply_armor_effects;
	std::unique_ptr<RenderHealthBarSystem> render_health_bar_system;
	std::unique_ptr<PlayerCollisionSystem> player_collision_system;
	std::unique_ptr<ManageInvincibleStatusSystem> manage_invincible_status_system;
	std::unique_ptr<DeathSystem> death_system;
	std::unique_ptr<ChangeMiningSizeSystem> change_mining_size_system;
	std::unique_ptr<ManageButtonActionsSystem> manage_button_actions_system;
	std::unique_ptr<ChestWindowSystem> chest_window_system;
	std::unique_ptr<RenderEssenceCounter> render_essence_counter;
	std::unique_ptr<CollectEssenceSystem> collect_essence_system;
	std::unique_ptr<OpenChestSystem> open_chest_system;

	//UI
	std::unique_ptr<CraftView> craft_view;
	std::shared_ptr<InventoryView> inventory_view;
	UISettings ui_settings;

	float ui_scale = 1.0f;

	//Enemies
	std::shared_ptr<EnemySpawnSystem> enemy_spawn_system;
	std::unique_ptr<PlayerComboSystem> player_combo_system;
	

	Entity player;

	GenerationData generation_data;

	//Text
	std::unique_ptr<graphics::Text> text;

	std::shared_ptr<graphics::SpriteAnimation> idle_animation;
	std::shared_ptr<graphics::SpriteAnimation> running_animation;
	std::shared_ptr<graphics::SpriteAnimation> jump_animation;
	std::shared_ptr<graphics::SpriteAnimation> fall_animation;
};
