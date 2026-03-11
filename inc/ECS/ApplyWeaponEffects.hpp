#pragma once

#include "Components.hpp"
#include <entt/entity/registry.hpp>

class ApplyWeaponEffects
{
public:
	ApplyWeaponEffects(entt::registry& registry) : registry{registry} {}

	template<typename Effect>
	Components::EffectDuration* isEffectAlreadyApplied(Entity target, Entity source)
	{
		auto view = registry.view<Components::EffectDuration, Effect>();
		for (auto [e, ef, effect] : view.each())
		{
			if (target == ef.target && source == ef.source) return &ef;
		}
		return nullptr;
	}

	void update(float dt)
	{
		std::vector<Entity> to_destroy;
		// Apply Effects
		{
			auto view = registry.view<Components::AddEffect, Components::WeaponEffects::Freeze>();
			for (auto [entity, add_effect_component, freeze_component] : view.each())
			{
				//Check if this effect is already applied from this source
				if (auto* effect_component = isEffectAlreadyApplied<Components::WeaponEffects::Freeze>(add_effect_component.target, add_effect_component.source))
				{
					//Restart the duration timer
					effect_component->timer = 0.0f;

					std::cout << "update timer" << std::endl;
					to_destroy.push_back(entity);
					continue;
				}

				//Make changes to the targets stats
				auto change_speed_entity = registry.create();
				registry.emplace<Components::ChangeSpeed>(change_speed_entity, add_effect_component.target, freeze_component.value * -1.0f);

				//Set the timer
				registry.emplace<Components::EffectDuration>(entity, add_effect_component.target, add_effect_component.source, freeze_component.duration, 0.0f);
				std::cout << "add new effect" << std::endl;

				//Remove AddEffect component
				registry.erase<Components::AddEffect>(entity);
			}
		}
		{
			auto view = registry.view<Components::AddEffect, Components::WeaponEffects::Poison>();
			for (auto [entity, add_effect_component, poison_component] : view.each())
			{
				//Check if this effect is already applied from this source
				if (auto* effect_component = isEffectAlreadyApplied<Components::WeaponEffects::Freeze>(add_effect_component.target, add_effect_component.source))
				{
					//Restart the duration timer
					effect_component->timer = 0.0f;

					to_destroy.push_back(entity);
					continue;
				}

				//Set the timer
				registry.emplace<Components::EffectDuration>(entity, add_effect_component.target, add_effect_component.source, poison_component.duration, 0.0f);
				std::cout << "add new effect" << std::endl;

				//Remove AddEffect component
				registry.erase<Components::AddEffect>(entity);
			}
		}

		// Update Effects
		{
			auto view = registry.view<Components::EffectDuration>();
			for (auto [entity, effect_duration_component] : view.each())
			{
				effect_duration_component.timer += dt;

				if (effect_duration_component.timer >= effect_duration_component.time)
				{
					registry.erase<Components::EffectDuration>(entity);
					registry.emplace<Components::RemoveEffect>(entity, effect_duration_component.target);
					std::cout << "Destroy effect" << std::endl;
				}
			}
		}

		// Remove Effects
		{
			auto view = registry.view<Components::RemoveEffect, Components::WeaponEffects::Freeze>();
			for (const auto& [entity, remove_effect_component, freeze_component] : view.each())
			{
				auto change_speed_entity = registry.create();
				registry.emplace<Components::ChangeSpeed>(change_speed_entity, remove_effect_component.target, freeze_component.value * 1.0f);

				to_destroy.push_back(entity);
			}

		}
		{
			auto view = registry.view<Components::RemoveEffect, Components::WeaponEffects::Poison>();
			for (const auto& [entity, remove_effect_component, poison_component] : view.each())
			{
				to_destroy.push_back(entity);
			}
		}
		for (const auto& entity : to_destroy) registry.destroy(entity);
	}

private:
	entt::registry& registry;
};