#pragma once
#include "SpriteAnimation.hpp"

class Inventory;

enum class ItemAction
{
	NONE,
	EQUIP,
	USE,
};

namespace Components
{

	struct Transform
	{
		glm::vec2 position;
		glm::vec2 size;
	};

	struct Renderable
	{
		//graphics::Sprite sprite;
		graphics::Sprite sprite;
		SDL_FlipMode flip_mode = SDL_FLIP_NONE;
		graphics::Color color = graphics::Color::WHITE;
	};

	struct Physics
	{
		bool can_move_horizontal = true;
		glm::vec2 velocity{};
		glm::vec2 acceleration{};
		glm::vec2 max_velocity{};
		float decelaration{};
		bool is_ground = false;
	};

	// Makes entities make a step when colliding with short walls
	struct PhysicStep
	{
		float max_step_height = 20.0f; ///< The maximum height of the wall that can trigger a step
	};

	struct Jump
	{
		float jump_force;
		bool jump_pressed_this_frame;
		bool jump_held;
	};

	struct MineIntent
	{
		glm::vec2 start_mouse_position;
		glm::vec2 current_mouse_position;
		bool active;
	};

	/*struct MineTilesAbility
	{
		float radius;
		float speed;
		int mine_size;
	};

	struct MineObjectsAbility
	{
		float radius;
		float speed;
	};*/

	struct MiningAbility
	{
		float speed;
		float radius;
		int size;
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

	struct AddItem
	{
		Entity target;
		Entity item;
	};

	struct PickUpItem
	{
		Entity target;
		Entity source;
		Entity item;
	};

	struct UseItem
	{
		size_t item_id;
	};

	// Notify that entity tries to equip an item
	struct EquipItem
	{
		Entity item;
	};

	struct UnequipItem
	{
		Entity item;
	};

	// Notify that item was equipped
	struct ItemEquipped
	{
		Entity item;
	};

	// Notify that item was unequipped
	struct ItemUnequipped
	{
		Entity item;
	};

	// Entity is going to drop an item
	struct DropItem
	{
		Entity target;
		Entity item;
	};

	// Dropped item that exists in the world
	struct DroppedItem
	{
		Entity item;
		bool can_be_collected = false;
		float timer = 0.0f; // Can be collected after some time
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

	struct Equipment
	{
		/*
		Item* helmet;
		Item* armor;
		Item* shoes;
		Item* amulet;
		std::vector<Item*> accessories;
		*/

		int max_weapon = 0;
		std::vector<Entity> weapons;
		std::optional<Entity> pickaxe = std::nullopt;
		std::optional<Entity> boots = std::nullopt;
	};

	struct Animation
	{
		
	};

	struct CharacterAnimation
	{
		std::shared_ptr<graphics::SpriteAnimation> idle_animation;
		std::shared_ptr<graphics::SpriteAnimation> running_animation;
		std::shared_ptr<graphics::SpriteAnimation> jump_animation;
		std::shared_ptr<graphics::SpriteAnimation> fall_animation;
	};

	struct EnemyAI
	{
		float position_update_timer;
		float position_update_time;
		glm::vec2 last_position;
	};

	struct Damage
	{
		Entity target;
		float value;
	};

	struct HitMark
	{
		float time = 0.5f;
		float timer = 0.0f;
		bool active = false;
	};

	namespace Effects
	{
		struct DoubleJump
		{
			bool is_active = true;
		};
	} // namespace Effects

	namespace InventoryItems
	{
		//STATIC information about an item
		struct ItemProperties
		{
			bool can_stack;
			int sprite_index;
			std::string name;
			ItemAction action = ItemAction::NONE;
		};

		//DYNAMIC information ek. stack size and is or is not equipped
		struct Item
		{
			size_t id;
			int stack_number = 1;
			bool equipped = false;
			float cooldown_timer = 0.0f;
			bool operator==(const Item& other) const { return this->id == other.id; }
		};

		struct UsableItem
		{
			
		};

		struct HealComponent
		{
			float value;
		};

		struct PickaxeComponent
		{
			float speed;
			float radius;
			int size;
		};

		struct WeaponComponent
		{
			float damage;
			float cooldown;
			float radius;
			float cooldown_timer = 0.0f;
		};

		struct ArmorComponent
		{
			
		};
		
		struct Boots
		{
			
		};

		struct AccessoryComponent
		{
			
		};
	} // namespace InventoryItems
} //namespace Components
