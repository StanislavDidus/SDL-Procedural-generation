#pragma once

#include <iostream>
#include <string>
#include "Item.hpp"
#include "glm/vec2.hpp"

struct ObjectProperties
{
	float durability;
	int sprite_index;
	std::string name;
	Item item;
	glm::vec2 size;
};

struct Object
{
	int id;
	float current_durability;
};