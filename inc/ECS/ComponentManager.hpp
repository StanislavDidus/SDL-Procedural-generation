#pragma once

#include <cstdint>
#include <unordered_map>

#include "glm/glm.hpp"
#include "ECS/Entity.hpp"
#include "Sprite.hpp"

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
};

struct PlaceIntent
{
	bool active;
};

struct PlaceAbility
{
	float radius;
};

struct Player
{

};


class ComponentManager
{
public:
	ComponentManager() = default;

	std::unordered_map<Entity, Transform> transform;
	std::unordered_map<Entity, Physics> physics;
	std::unordered_map<Entity, Jump> jump;
	std::unordered_map<Entity, Player> player; 
	std::unordered_map<Entity, MineAbility> mine_ability;
	std::unordered_map<Entity, MineIntent> mine_intent;
	std::unordered_map<Entity, PlaceAbility> place_ability;
	std::unordered_map<Entity, PlaceIntent> place_intent;
};