#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "ObjectManager.hpp"

class ObjectDurabilityDisplay
{
public:
    ObjectDurabilityDisplay(entt::registry& registry) 
        : registry{registry} {}
    ~ObjectDurabilityDisplay() = default;
    
    void update()
    {
        auto view = registry.view<Components::Object, Components::Renderable>();
        for (auto [entity, object_component, renderable_component] : view.each())
        {
            float max_durability = ObjectManager::get().getProperties(object_component.id).durability;
            float current_durability = object_component.current_durability;
            float percentage = std::clamp(1.0f - current_durability / max_durability, 0.0f, 1.0f);
            uint8_t value = static_cast<uint8_t>(255 * (1.0f - percentage));
            //graphics::Color color_value = graphics::Color::WHITE + (graphics::Color::BLACK - graphics::Color::WHITE) * percentage;
            renderable_component.color = {value, value, value, 255};
            //renderable_component.color = color_value;
        }
    }
private:
    entt::registry& registry;
};
