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
	float rotation;

};
struct Renderable
{
	Sprite sprite;
};

struct Physics
{
	glm::vec2 velocity;
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
	std::unordered_map<Entity, Player> player;
};