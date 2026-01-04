#pragma once

#include <cstdint>
#include <unordered_map>

#include "glm/glm.hpp"
#include "ECS/Entity.hpp"
#include "Sprite.hpp"

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
		bool active;
	};

	struct MineAbility
	{
		float radius;
		float speed;
		int mine_size;
	};

	struct PlaceIntent
	{
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

}

using namespace Components;

class ComponentManager
{
public:
	ComponentManager() = default;

	std::unordered_map<Entity, Transform> transform;
	std::unordered_map<Entity, Physics> physics;
	std::unordered_map<Entity, Jump> jump;
	std::unordered_map<Entity, Player> player; 
	std::unordered_map<Entity, Health> health;
	std::unordered_map<Entity, MineAbility> mine_ability;
	std::unordered_map<Entity, MineIntent> mine_intent;
	std::unordered_map<Entity, PlaceAbility> place_ability;
	std::unordered_map<Entity, PlaceIntent> place_intent;
	std::unordered_map<Entity, HasInventory> has_inventory;
};