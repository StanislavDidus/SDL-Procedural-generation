#pragma once

#include <string>
#include <vector>
#include <memory>

struct ItemComponent
{
	ItemComponent() = default;
	virtual ~ItemComponent() = default;
};

struct ItemProperties
{
	bool can_stack;
	int sprite_index;
	std::string name;
	std::vector<std::shared_ptr<ItemComponent>> components;
};

struct Item
{
	int id;
	int stack_number;

	bool operator==(const Item& other) const { return this->id == other.id; }
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