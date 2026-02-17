#pragma once

#include <unordered_map>

#include "ECS/Entity.hpp"
#include "Sprite.hpp"
#include "glm/vec2.hpp"
#include "ECS/Components.hpp"

class Inventory;

using namespace Components;

/// <summary>
/// Manager of components.
/// </summary>
class ComponentManager
{
public:

	static ComponentManager& get()
	{
		static ComponentManager s;
		return s;
	}

	std::unordered_map<Entity, Transform> transform;
	std::unordered_map<Entity, Physics> physics;
	std::unordered_map<Entity, Jump> jump;
	std::unordered_map<Entity, Renderable> renderable;
	std::unordered_map<Entity, CharacterAnimation> character_animations;
	std::unordered_map<Entity, Player> player; 
	std::unordered_map<Entity, Health> health;
	std::unordered_map<Entity, CraftItem> craft_item;
	std::unordered_map<Entity, CraftingAbility> crafting_ability;
	std::unordered_map<Entity, EnemyAI> enemy_ai;
	std::unordered_map<Entity, std::vector<Damaged>> damaged;
	std::unordered_map<Entity, HitMark> hit_mark;

	//Button control
	std::unordered_map<Entity, Components::Button> button;
	std::unordered_map<Entity, ButtonEntered> button_entered;
	std::unordered_map<Entity, ButtonCovered> button_covered;
	std::unordered_map<Entity, ButtonExit> button_exit;
	std::unordered_map<Entity, ButtonHeld> button_held;
	std::unordered_map<Entity, ButtonReleased> button_released;

	std::unordered_map<Entity, CraftButton> craft_button;

	//std::unordered_map<Entity, MineTilesAbility> mine_tiles_ability;
	//std::unordered_map<Entity, MineObjectsAbility> mine_objects_ability;
	std::unordered_map<Entity, MiningAbility> mining_ability;
	std::unordered_map<Entity, MineObjectsState> mine_objects_state;
	std::unordered_map<Entity, MineObjectsStarted> mine_objects_started;
	std::unordered_map<Entity, MineObjectsFinished> mine_objects_finished;

	std::unordered_map<Entity, MineIntent> mine_intent; ///< Shows the <b>entity's</b> intent to mine.
	std::unordered_map<Entity, PlaceAbility> place_ability;
	std::unordered_map<Entity, PlaceIntent> place_intent;
	std::unordered_map<Entity, HasInventory> has_inventory;
	std::unordered_map<Entity, UseItem> use_item;
	std::unordered_map<Entity, EquipItem> equip_item;
	std::unordered_map<Entity, UnequipItem> unequip_item;
	std::unordered_map<Entity, DropItem> drop_item;

	//std::unordered_map<Entity, Weapons> circle_slots;
	//std::unordered_map<Entity, Pickaxe> pickaxe;
	std::unordered_map<Entity, Equipment> equipment;

private:
	ComponentManager() = default;
};