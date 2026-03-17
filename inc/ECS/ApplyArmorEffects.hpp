#pragma once
#include <entt/entity/registry.hpp>
#include "Components.hpp"
#include "Entity.hpp"
#include "Systems.hpp"

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
			auto target = item_equipped_component.target;
			applyEffect<Components::Effects::HealthBonus>(item_equipped_component.item, target);
			applyEffect<Components::Effects::Big>(item_equipped_component.item, target);
			applyEffect<Components::Effects::DecreaseGravity>(item_equipped_component.item, target);
			applyEffect<Components::Effects::DoubleJump>(item_equipped_component.item, target);
			applyEffect<Components::WeaponEffects::SpeedBoost>(item_equipped_component.item, target);
			applyEffect<Components::Effects::IncreaseWeaponSlots>(item_equipped_component.item, target);
			applyEffect<Components::Effects::Spike>(item_equipped_component.item, target);
		}

		//Unequip
		auto view_unequipped = registry.view<Components::ItemUnequipped>();
		for (auto [entity, item_unequipped_component] : view_unequipped.each())
		{
			removeEffectFromSource(registry, item_unequipped_component.target, item_unequipped_component.item);
		}
	}

private:
	template<typename T>
	void applyEffect(Entity item, Entity target) const
	{
		if (registry.all_of<T>(item))
		{
			auto& component = registry.get<T>(item);
			auto effect_entity = registry.create();
			registry.emplace<T>(effect_entity, component);
			registry.emplace<Components::WeaponEffects::Effect>(effect_entity, target, item);
		}
	}
	entt::registry& registry;
};
