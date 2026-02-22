#pragma once
#include <entt/entity/registry.hpp>
#include "Components.hpp"
#include "Entity.hpp"

class ApplyArmorEffects
{
public:
	ApplyArmorEffects(entt::registry& registry) : registry{registry} {}

	void update()
	{
		//Equip
		auto view_equipped = registry.view<Components::ItemEquipped>();
		for (auto [entity, item_equipped_component] : view_equipped.each())
		{
			if (registry.all_of<Components::Effects::DoubleJump>(item_equipped_component.item))
			{
				registry.emplace_or_replace<Components::Effects::DoubleJump>(entity);
				double_jump_counter[entity]++;
			}
			//else if(registry.all_of<Components::Effects::Regeneration>(item)
			// Do something
		}

		//PREFERRED
		// Player(Entity) -> has ItemEquipped component with Item(Entity)
		// Item(Entity) -> ItemEquipped with Player(Entity(
		// view<ItemEquipped, DoubleJump>();

		//Unequip
		auto view_unequipped = registry.view<Components::ItemUnequipped>();
		for (auto [entity, item_unequipped_component] : view_unequipped.each())
		{
			if (registry.all_of<Components::Effects::DoubleJump>(item_unequipped_component.item))
			{
				if (double_jump_counter[entity] <= 1)
				{
					registry.remove<Components::Effects::DoubleJump>(entity);
					std::cout << "Component deleted" << std::endl;
				}
				double_jump_counter[entity]--;
			}
		}
	}

private:
	entt::registry& registry;

	std::unordered_map<Entity, int> double_jump_counter;
};
