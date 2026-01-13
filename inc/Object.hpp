#pragma once

#include <iostream>
#include <string>
#include "Item.hpp"
#include "glm/vec2.hpp"
#include "SDL3/SDL_rect.h"

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
	glm::vec2 position;
	bool is_destroyed;
	bool received_damage_last_frame;

	void dealDamage(float damage)
	{
		current_durability -= damage;
		received_damage_last_frame = true;

		if (current_durability <= 0.f) is_destroyed = true;
	}

	bool operator==(const Object& rhs) const { return this->id == rhs.id; }
};