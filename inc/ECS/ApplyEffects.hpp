#pragma once

#include "Components.hpp"
#include <entt/entity/registry.hpp>
#include "ECS/Systems.hpp"

class ApplyEffects
{
public:
	ApplyEffects(entt::registry& registry) : registry{registry} {}
	
	void update(float dt)
	{
		//Damage entities with poison
		{
			/*
			auto view = registry.view<Components::Health>();
			for (auto [entity, health_component] : view.each())
			{
				for (const auto& effect : getEffects<Components::WeaponEffects::Poison>(registry, entity))
				{
					const auto& effect_component = registry.get<Components::WeaponEffects::Effect>(effect);
					auto& poison_component = registry.get<Components::WeaponEffects::Poison>(effect);

					poison_component.timer += dt;
					if (poison_component.timer >= 1.0f)
					{
						poison_component.timer = 0.0f;

						auto damage_entity = registry.create();
						// Set damage source to entt::null so that poison doesn't apply twice
						registry.emplace<Components::Damage>(damage_entity, entity, entt::null, poison_component.value);
					}
				}
			}
		*/
		}

		{
			auto view = registry.view<Components::Equipment, Components::BaseValues>();
			for (auto [entity, equipment_component, base_value] : view.each())
			{
				int value = 0;
				for (const auto& effect : getEffects<Components::Effects::IncreaseWeaponSlots>(registry, entity))
				{
					value += registry.get<Components::Effects::IncreaseWeaponSlots>(effect).value;
				}

				equipment_component.max_weapon = base_value.max_weapons + value;
			}
		}
		{
			auto view = registry.view<Components::Physics>();
			for (auto [entity, physics_component] : view.each())
			{
				//TODO: Fix stun logic
				// Player should not move when mining objects
				if (isEffectApplied<Components::WeaponEffects::Stun>(registry, entity))
				{
					physics_component.can_move_horizontal = false;
				}
			}
		}

		{
			auto view = registry.view<Components::Health, Components::BaseValues>();
			for (auto [entity, health_component, base_value] : view.each())
			{
				float value = 0.0f;
				for (const auto& effect : getEffects<Components::Effects::HealthBonus>(registry, entity))
				{
					value += registry.get<Components::Effects::HealthBonus>(effect).value;
				}
				health_component.max_health = base_value.max_health + value;
			}
		}
		{
			auto view = registry.view<Components::Transform, Components::BaseValues>();
			for (auto [entity, transform_component, base_value] : view.each())
			{
				float  value = 1.0f;
				for (const auto& effect : getEffects<Components::Effects::Big>(registry, entity))
				{
					float val = registry.get<Components::Effects::Big>(effect).value;
					value *= 1.f + val / 100.0f;
				}
				transform_component.size = base_value.size * value;

			}
		}
		{
			auto view = registry.view<Components::Physics, Components::BaseValues>();
			for (auto [entity, physics_component, base_value] : view.each())
			{
				float value = 1.0f;
				for (const auto& effect : getEffects<Components::Effects::DecreaseGravity>(registry, entity))
				{
					float val = registry.get<Components::Effects::DecreaseGravity>(effect).value;
					value *= 1.f - val / 100.0f;
				}

				physics_component.gravity = base_value.gravity * value;
			}
		}
		{
			auto view = registry.view<Components::Jump>();
			for (auto [entity, jump] : view.each())
			{
				const auto& double_jump_components = getEffects<Components::Effects::DoubleJump>(registry, entity);
				int count = double_jump_components.size() + 1;
				jump.max_jump_count = count;
			}
		}
		{
			auto view = registry.view<Components::Physics, Components::BaseValues>();
			for (auto [entity, physics, base_value] : view.each())
			{
				float value = 1.0f;
				for (const auto& effect : getEffects<Components::WeaponEffects::SpeedBoost>(registry, entity))
				{
					float val = registry.get<Components::WeaponEffects::SpeedBoost>(effect).value;
					value *= 1.f + val / 100.0f;
				}
				for (const auto& effect : getEffects<Components::WeaponEffects::Freeze>(registry, entity))
				{
					float val = registry.get<Components::WeaponEffects::Freeze>(effect).value;
					value *= 1.f - val / 100.0f;
				}

				physics.acceleration = base_value.acceleration * value;
				physics.max_velocity = base_value.max_velocity * value;
			}
		}

		{
			auto view = registry.view<Components::Health>();
			for (auto [entity, health_component] : view.each())
			{
				for (auto& effect_entity : getEffects<Components::WeaponEffects::Poison>(registry, entity))
				{
					auto& effect = registry.get<Components::WeaponEffects::Poison>(effect_entity);
					effect.timer += dt;
					if (effect.timer >= 1.0f)
					{
						effect.timer = 0.0f;
						auto damage_entity = registry.create();
						registry.emplace<Components::Damage>(damage_entity, entity, entt::null, effect.value);
					}
				}
			}
		}
	}

	private:
	entt::registry& registry;
};