#pragma once
#include "Inventory.hpp"

namespace Components
{

	struct Transform
	{
		glm::vec2 position;
		glm::vec2 size;
	};

	struct Renderable
	{
		graphics::Sprite sprite;
		SDL_FlipMode flip_mode = SDL_FLIP_NONE;
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
		size_t recipe_id; ///< ID of the recipe according to CraftingManager.
	};

	struct CraftItem
	{
		size_t recipe_id;
	};

	struct CraftingAbility
	{
		std::vector<size_t> recipes_acquired;
	};

	struct Pickaxe
	{
		Item item;
	};
} //namespace Components
