#pragma once
#include "Entity.hpp"
#include "RandomizedItem.hpp"
#include "SpriteAnimation.hpp"

class Inventory;

enum class ItemAction
{
	NONE,
	EQUIP,
	USE,
};

enum class EssenceType
{
	NONE,
	COMMON,
	SNOW,
	SAND,
};

namespace Components
{
	struct BaseValues
	{
		glm::vec2 size;
		glm::vec2 acceleration;
		glm::vec2 max_velocity;
	};

	struct Transform
	{
		glm::vec2 position;
		glm::vec2 size;

		glm::vec2 base_size = size;
	};

	struct Renderable
	{
		graphics::Sprite sprite;
		int priority = 0;
		SDL_FlipMode flip_mode = SDL_FLIP_NONE;
		graphics::Color color = graphics::Color::WHITE;
		bool ignore_view_zoom = false;
	};

	struct Physics
	{
		bool can_move_horizontal = true;
		glm::vec2 velocity{};
		glm::vec2 acceleration{};
		glm::vec2 max_velocity{};
		float decelaration{};
		bool is_ground = false;

		glm::vec2 base_acceleration = acceleration;
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
		int max_size;
		int current_size;
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

	struct Regeneration
	{
		float speed = 0.0f; ///< Health regenerated in 1 second.
		float timer = 0.0f;
	};

	struct Invincible
	{
		float time = 0.0f;
		float timer = 0.0f;
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

	// Notify that target tries to equip an item
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

	struct DropItem2
	{
		glm::vec2 position;
		Entity item;
	};

	// Dropped item that exists in the world
	struct DroppedItem
	{
		Entity item;
		bool can_be_collected = false;
		float timer = 0.0f; // Can be collected after some time
		float time = 0.1f;
	};

	struct DropItemChest
	{
		Entity item;

		float timer = 0.0f;
		float last_update = 0.0f;
		int last_sprite = 0;
	};
	
	struct Button
	{
		//global identifies if a position of this button is in global or local coordinates
		//where local are usually used for ui
		bool global = false;
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

	struct ButtonFunction
	{
		std::function<void()> command;
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
		int max_weapon = 0;
		std::vector<Entity> weapons;
		Entity pickaxe = entt::null;
		Entity helmet = entt::null;
		Entity armor = entt::null;
		Entity boots = entt::null;
		std::vector<Entity> accessories;
	};

	struct EquipmentEssence
	{
		int common_essence = 0;
		int snow_essence = 0;
		int sand_essence = 0;
	};

	struct GiveEssence
	{
		EssenceType type = EssenceType::NONE;
		int number = 0;
		float chance = 1.0f;
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
	


	struct Damage
	{
		Entity target;
		Entity source;
		float value;
	};

	struct HitMark
	{
		float time = 0.5f;
		float timer = 0.0f;
		bool active = false;
		graphics::Color base_color = graphics::Color::WHITE;
	};

	struct HealthBar
	{
		glm::vec2 position;
		glm::vec2 size;
		graphics::Color color;
	};

	struct CollideDamage
	{
		float damage;
	};

	struct Dead
	{
		
	};

	struct Chest
	{
		size_t base_item;
		size_t common_item;
		size_t snow_item;
		size_t sand_item;
	};

	struct Closed_Chest
	{
		
	};

	namespace Enemies
	{
		struct Enemy
		{
			size_t id;
		};

		/// <summary>
		/// 
		/// </summary>
		struct EnemyData
		{
			std::string name;
			
			int sprite_index;
			bool is_aggressive;
			float max_health;

			//AI settings
			float ai_efficiency;

			std::vector<RandomizedItem> item_drop;
		};

		/// <summary>
		/// 
		/// </summary>
		struct EnemySpawnInfo
		{
			size_t id;
			float spawn_chance_weight;
			glm::ivec2 size;
			std::vector<size_t> spawn_tiles;
		};

		struct EnemyAI
		{
			float position_update_timer;
			float position_update_time;
			glm::vec2 last_position;
		};

		struct DropEssence
		{
			EssenceType type = EssenceType::NONE;
			int number = 0;
			float chance = 0.0f;
		};
	} // namespace Enemies

	namespace Effects
	{
		struct DoubleJump
		{
			bool is_active = true;
		};

		struct Big
		{
			float value = 1.0f;
		};

		struct Speed
		{
			float value = 1.0f;
		};
		
		struct HealthBonus
		{
			float value = 0.0f;
		};
	} // namespace Effects

	struct AddEffect
	{
		Entity target;
		Entity source;
	};

	struct EffectDuration
	{
		Entity target;
		Entity source;

		float time = 0.0f;
		float timer = 0.0f;
	};

	struct RemoveEffect
	{
		Entity target;
	};

	struct ChangeSpeed
	{
		Entity target;
		float value = 0.0f; // Value in percents. Positive - increase speed. Negative - decrease speed.
	};

	namespace WeaponEffects
	{
		struct Freeze
		{
			float value = 0.0f;
			float duration = 0.0f;
		};

		struct Poison
		{
			float value = 0.0f; // Damage per second
			float duration = 0.0f;
			
			float timer = 0.0f;
		};
	}

	namespace UI
	{
		struct OpenChestWindow
		{
			Entity target;
			Entity chest;
		};

		struct OpenChest
		{
			Entity chest;
			EssenceType type;
			int number;
		};

		struct Window
		{

		};

		struct ChestWindow
		{
			Entity target;
			Entity chest;

			EssenceType essence_type = EssenceType::NONE;
			int essence_number = 0;
		};

		struct ParentWindow
		{
			std::vector<Entity> children;
		};
	} // namespace UI

	namespace InventoryItems
	{
		//STATIC information about an item
		struct ItemProperties
		{
			bool can_stack;
			int sprite_index;
			std::string name;
			ItemAction action = ItemAction::NONE;
			std::string description;
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

		struct Helmet
		{
			
		};

		struct Armor
		{
			
		};
		
		struct Boots
		{
			
		};

		struct Accessory
		{
			
		};
	} // namespace InventoryItems
} //namespace Components
