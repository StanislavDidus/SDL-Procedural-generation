#pragma once

#include <unordered_map>

#include "EntityManager.hpp"
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
	std::unordered_map<Entity, PhysicStep> physic_step;
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
	std::unordered_map<Entity, AddItem> add_item;
	std::unordered_map<Entity, UseItem> use_item;
	std::unordered_map<Entity, EquipItem> equip_item;
	std::unordered_map<Entity, UnequipItem> unequip_item;
	std::unordered_map<Entity, ItemEquipped> item_equipped;
	std::unordered_map<Entity, ItemUnequipped> item_unequipped;
	std::unordered_map<Entity, DropItem> drop_item;
	std::unordered_map<Entity, DroppedItem> dropped_item;

	//std::unordered_map<Entity, Weapons> circle_slots;
	//std::unordered_map<Entity, Pickaxe> pickaxe;
	std::unordered_map<Entity, Equipment> equipment;

private:
	ComponentManager() = default;
};

// Removes entity from the ECS entirely (cleans all components and frees the id from EntityManager)
static void removeEntity(Entity entity)
{
	auto& component_manager = ComponentManager::get();
	auto& entity_manager = EntityManager::get();

	component_manager.transform.erase(entity);
	component_manager.physics.erase(entity);
	component_manager.physic_step.erase(entity);
	component_manager.jump.erase(entity);
	component_manager.renderable.erase(entity);
	component_manager.character_animations.erase(entity);
	component_manager.player.erase(entity);
	component_manager.health.erase(entity);
	component_manager.craft_item.erase(entity);
	component_manager.crafting_ability.erase(entity);
	component_manager.enemy_ai.erase(entity);
	component_manager.damaged.erase(entity);
	component_manager.hit_mark.erase(entity);
	component_manager.button.erase(entity);
	component_manager.button_entered.erase(entity);
	component_manager.button_covered.erase(entity);
	component_manager.button_exit.erase(entity);
	component_manager.button_held.erase(entity);
	component_manager.button_released.erase(entity);
	component_manager.craft_button.erase(entity);
	component_manager.mining_ability.erase(entity);
	component_manager.mine_objects_state.erase(entity);
	component_manager.mine_objects_started.erase(entity);
	component_manager.mine_objects_finished.erase(entity);
	component_manager.mine_intent.erase(entity);
	component_manager.place_ability.erase(entity);
	component_manager.place_intent.erase(entity);
	component_manager.has_inventory.erase(entity);
	component_manager.add_item.erase(entity);
	component_manager.use_item.erase(entity);
	component_manager.equip_item.erase(entity);
	component_manager.unequip_item.erase(entity);
	component_manager.item_equipped.erase(entity);
	component_manager.item_unequipped.erase(entity);
	component_manager.drop_item.erase(entity);
	component_manager.dropped_item.erase(entity);
	component_manager.equipment.erase(entity);

	entity_manager.destroyEntity(entity);
}