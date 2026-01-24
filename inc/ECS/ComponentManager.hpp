#pragma once

#include <unordered_map>

#include "ECS/Entity.hpp"
#include "Sprite.hpp"
#include "glm/vec2.hpp"

class Inventory;

namespace Components
{

	struct Transform
	{
		glm::vec2 position;
		glm::vec2 size;
	};

	struct Renderable
	{
		Sprite sprite;
	};

	struct Physics
	{
		bool can_move_horizontal = true;
		glm::vec2 velocity;
		glm::vec2 acceleration;
		glm::vec2 max_velocity;
		float decelaration;
		bool is_ground;
		float step;
	};

	struct Jump
	{
		float jump_force;
		bool jump_ready;
	};

	struct MineIntent
	{
		glm::vec2 start_mouse_position;
		glm::vec2 current_mouse_position;
		bool active;
	};

	struct MineTilesAbility
	{
		float radius;
		float speed;
		int mine_size;
	};

	struct MineObjectsAbility
	{
		float radius;
		float speed;
	};

	struct MineObjectsState
	{

	};

	struct MineObjectsStarted
	{

	};

	struct MineObjectsFinished
	{

	};

	struct PlaceIntent
	{
		glm::vec2 target_global_position;
		bool active;
	};

	struct PlaceAbility
	{
		float radius;
		float placing_time;
		float placing_timer;
	};

	struct Player
	{
		
	};

	struct Health
	{
		float max_health;
		float current_health;
	};

	struct HasInventory
	{
		std::shared_ptr<Inventory> inventory;
	};

	struct Button
	{
		
	};

	struct ButtonEntered
	{
		
	};

	struct ButtonCovered ///< Button is covered by a cursor.
	{
		
	};

	struct ButtonExit ///< Button is covered by a cursor.
	{

	};

	struct ButtonHeld
	{
		
	};

	struct ButtonReleased
	{
		
	};

	struct CraftButton
	{
		bool is_available; ///< Shows if the recipe is unlocked and can be crafted.
		size_t recipe_id;
	};

	struct CraftItem
	{
		size_t recipe_id;
	};

	struct CraftingAbility
	{
		std::vector<size_t> recipes_acquired;
	};
}

using namespace Components;

/// <summary>
/// Manager of components.
/// </summary>
class ComponentManager
{
public:
	ComponentManager() = default;

	std::unordered_map<Entity, Transform> transform;
	std::unordered_map<Entity, Physics> physics;
	std::unordered_map<Entity, Jump> jump;
	std::unordered_map<Entity, Player> player; 
	std::unordered_map<Entity, Health> health;
	std::unordered_map<Entity, CraftItem> craft_item;
	std::unordered_map<Entity, CraftingAbility> crafting_ability;
	
	//Button control
	std::unordered_map<Entity, Button> button;
	std::unordered_map<Entity, ButtonEntered> button_entered;
	std::unordered_map<Entity, ButtonCovered> button_covered;
	std::unordered_map<Entity, ButtonExit> button_exit;
	std::unordered_map<Entity, ButtonHeld> button_held;
	std::unordered_map<Entity, ButtonReleased> button_released;

	std::unordered_map<Entity, CraftButton> craft_button;

	std::unordered_map<Entity, MineTilesAbility> mine_tiles_ability;
	std::unordered_map<Entity, MineObjectsAbility> mine_objects_ability;
	std::unordered_map<Entity, MineObjectsState> mine_objects_state;
	std::unordered_map<Entity, MineObjectsStarted> mine_objects_started;
	std::unordered_map<Entity, MineObjectsFinished> mine_objects_finished;

	std::unordered_map<Entity, MineIntent> mine_intent; ///< Shows the <b>entity's</b> intent to mine.
	std::unordered_map<Entity, PlaceAbility> place_ability;
	std::unordered_map<Entity, PlaceIntent> place_intent;
	std::unordered_map<Entity, HasInventory> has_inventory;
};