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

struct Mining
{

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
};