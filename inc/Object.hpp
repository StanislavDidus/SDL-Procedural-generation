#pragma once

#include <iostream>
#include <string>
#include "RandomizedItem.hpp"
#include "glm/vec2.hpp"

struct ObjectProperties
{
	float durability;
	int sprite_index;
	std::string name;
	std::vector<RandomizedItem> drop;
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