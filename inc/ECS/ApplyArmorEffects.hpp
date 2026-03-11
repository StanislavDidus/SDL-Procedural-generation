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
			else if (registry.all_of<Components::Effects::HealthBonus>(item_equipped_component.item) && registry.all_of<Components::Health>(entity))
			{
				auto& health_component = registry.get<Components::Health>(entity);	
				const auto& health_bonus_component = registry.get<Components::Effects::HealthBonus>(item_equipped_component.item);

				health_component.max_health += health_bonus_component.value;
			}
			else if (registry.all_of<Components::Effects::Big>(item_equipped_component.item) && registry.all_of<Components::Transform>(entity) && registry.all_of<Components::BaseValues>(entity))
			{
				auto& transform_component = registry.get<Components::Transform>(entity);
				const auto& base_values_component = registry.get<Components::BaseValues>(entity);
				const auto& value = registry.get<Components::Effects::Big>(item_equipped_component.item).value;

				glm::vec2 value_to_add = base_values_component.size * (value - 1);
				transform_component.size += value_to_add;

			}
			else if (registry.all_of<Components::Effects::Speed>(item_equipped_component.item) && registry.all_of<Components::Physics>(entity) && registry.all_of<Components::BaseValues>(entity))
			{
				auto& physics_component = registry.get<Components::Physics>(entity);
				const auto& base_values_component = registry.get<Components::BaseValues>(entity);
				const auto& value = registry.get<Components::Effects::Speed>(item_equipped_component.item).value;

				auto change_speed = registry.create();
				registry.emplace<Components::ChangeSpeed>(change_speed, entity, value);
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
			if (registry.all_of<Components::Effects::HealthBonus>(item_unequipped_component.item) && registry.all_of<Components::Health>(entity))
			{
				
				auto& health_component = registry.get<Components::Health>(entity);	
				const auto& health_bonus_component = registry.get<Components::Effects::HealthBonus>(item_unequipped_component.item);

				health_component.max_health -= health_bonus_component.value;
			}
			else if (registry.all_of<Components::Effects::Big>(item_unequipped_component.item) && registry.all_of<Components::Transform>(entity) && registry.all_of<Components::BaseValues>(entity))
			{
				auto& transform_component = registry.get<Components::Transform>(entity);
				const auto& base_values_component = registry.get<Components::BaseValues>(entity);
				const auto& value = registry.get<Components::Effects::Big>(item_unequipped_component.item).value;

				glm::vec2 value_to_add = base_values_component.size * (value - 1);
				transform_component.size -= value_to_add;

			}
			else if (registry.all_of<Components::Effects::Speed>(item_unequipped_component.item) && registry.all_of<Components::Physics>(entity) && registry.all_of<Components::BaseValues>(entity))
			{
				auto& physics_component = registry.get<Components::Physics>(entity);
				const auto& base_values_component = registry.get<Components::BaseValues>(entity);
				const auto& value = registry.get<Components::Effects::Speed>(item_unequipped_component.item).value;

				auto change_speed = registry.create();
				registry.emplace<Components::ChangeSpeed>(change_speed, entity, -value);
			}
		}
	}

private:
	entt::registry& registry;

	std::unordered_map<Entity, int> double_jump_counter;
};
