#pragma once

#include <string>
#include <vector>
#include <memory>

struct ItemProperties
{
	bool can_stack;
	int stack_number;
	int sprite_index;
	std::string name;
};

struct ItemComponent 
{
	ItemComponent() = default;
};

struct Item
{
	ItemProperties properties;
	std::vector<std::shared_ptr<ItemComponent>> components;

	bool operator==(const Item& other) const { return this->properties.name == other.properties.name; }
};

enum class Effect
{
	SPEED,
	MINING_RANGE,
	MINING_SPEED,
	JUMP_HEIGHT,
	STRENGTH,
	HEALTH_REGENERATION,
};

namespace ItemComponents
{
	struct Usable : public ItemComponent
	{

	};

	struct Heal : public ItemComponent
	{
		Heal(int value) : value(value) {}

		int value;
	};

	struct AddEffect : public ItemComponent
	{
		AddEffect(Effect effect, float duration) : effect(effect), duration(duration) {}

		Effect effect;
		float duration;
	};
}