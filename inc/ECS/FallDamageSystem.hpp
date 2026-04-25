#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"

class FallDamageSystem
{
public:
    FallDamageSystem(entt::registry& registry)
        : registry{registry} {}
    ~FallDamageSystem() = default;
    
    void update()
    {
        auto view = registry.view<Components::Transform, Components::Physics, Components::FallDamage, Components::Health>();
        for (auto [entity, transform_component, physics_components, fall_damage_component, health_component] : view.each())
        {
            if (physics_components.is_ground)
            {
                float distance_y = transform_component.position.y - fall_damage_component.last_position.y;
            
                if (distance_y > fall_damage_component.fall_damage_threshold)
                {
                    Entity damage_entity = registry.create();
                    registry.emplace<Components::Damage>(damage_entity, entity, damage_entity, distance_y * fall_damage_component.damage_per_height);
                }
                
                fall_damage_component.last_position = transform_component.position;
            }
        }
    }
private:
    entt::registry& registry;
};
