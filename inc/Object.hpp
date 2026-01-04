#pragma once

#include <iostream>
#include <string>
#include "Item.hpp"

struct ObjectProperties
{
	float durability;
	int sprite_index;
	std::string name;
	Item item;
};

struct Object
{
	int id;
	float current_durability;
};