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
	ItemProperties(bool can_stack, int sprite_index, const std::string& name, std::vector<std::unique_ptr<ItemComponent>> components);

	bool can_stack;
	int sprite_index;
	std::string name;
	std::vector<std::unique_ptr<ItemComponent>> components;
};

struct Item
{
	size_t id;
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

	struct Pickaxe : public ItemComponent
	{
		Pickaxe(float mining_speed, float mining_radius, int mining_size)
			: speed(mining_speed), radius(mining_radius), size(mining_size) {}

		float speed;
		float radius;
		int size;
	};
}