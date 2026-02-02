#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

struct HealData
{
	float amount;
};

struct PickaxeData
{
	float speed;
	float radius;
	int size;
};

struct MeleeWeaponData
{
	float damage;
	float cooldown;
	float radius;
};

struct ItemComponent
{
	explicit ItemComponent(int number_properties) : number_properties(number_properties) {};
	virtual ~ItemComponent() = default;

	int number_properties = 0;
};

enum class ItemAction
{
	NONE,
	EQUIP,
	USE,
};

template<typename T>
static T* getItemComponent(const std::vector<std::unique_ptr<ItemComponent>>& components)
{
	auto it = std::ranges::find_if(components, [](const std::unique_ptr<ItemComponent>& other)
		{
			return	dynamic_cast<T*>(other.get()) != nullptr;
		});

	return it != components.end() ? static_cast<T*>(it->get()) : nullptr;
}

struct ItemProperties
{
	/*ItemProperties(bool can_stack, int sprite_index, const std::string& name, ItemAction action, std::optional<HealData> heal_data, std::optional<PickaxeData> pickaxe_data, std::optional<MeleeWeaponData> melee_weapon_data);*/

	bool can_stack;
	int sprite_index;
	std::string name;

	ItemAction action = ItemAction::NONE;

	std::optional<HealData> heal_data;
	std::optional<PickaxeData> pickaxe_data;
	std::optional<MeleeWeaponData> melee_weapon_data;
};

struct Item
{
	size_t id;
	int stack_number;
	bool equipped = false;

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
		Usable(int number_properties) : ItemComponent(number_properties) {}
	};

	struct Heal : public ItemComponent
	{
		Heal(int number_propertie, int value) : ItemComponent(number_propertie), value(value) {}

		int value;
	};

	struct AddEffect : public ItemComponent
	{
		AddEffect(int number_propertie, Effect effect, float duration) : ItemComponent(number_propertie), effect(effect), duration(duration) {}

		Effect effect;
		float duration;
	};

	struct Pickaxe : public ItemComponent
	{
		Pickaxe(int number_propertie, float mining_speed, float mining_radius, int mining_size)
			: ItemComponent(number_propertie), speed(mining_speed), radius(mining_radius), size(mining_size) {}

		float speed;
		float radius;
		int size;
	};

	struct MeleeWeapon : public ItemComponent
	{
		MeleeWeapon(int number_propertie, float damage, float cooldown, float radius)
			: ItemComponent(number_propertie), damage(damage), cooldown(cooldown), radius(radius) {
		}

		float damage;
		float cooldown;
		float radius;
	};
}