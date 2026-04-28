#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"
#include "GpuRenderer.hpp"
#include "ResourceManager.hpp"

class ShowMessageSystem
{
public:
    ShowMessageSystem(entt::registry& registry)
        : registry{registry} {}
    ~ShowMessageSystem() = default;
    
    void update(float dt)
    {
        auto view = registry.view<Components::Transform, Components::UI::ShowMessage>();
        for (auto [entity, transform_component, show_message_component] : view.each())
        {
            show_message_component.timer += dt;
            if (show_message_component.timer >= show_message_component.time)
            {
                registry.remove<Components::UI::ShowMessage>(entity);
            }
        }
    }
    
    void render(graphics::GpuRenderer& screen) const
    {
        const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Dialogue_Window");
        const auto& font = ResourceManager::get().getFont("Main");
        
        float message_width = 170.0f;
        float message_height = 150.0f;
        
        auto view = registry.view<Components::Transform, Components::UI::ShowMessage>();
        for (auto [entity, transform_component, show_message_component] : view.each())
        {
            graphics::drawScaledSprite(
                screen,
                sprite,
                transform_component.position.x,
                transform_component.position.y - message_height,
                message_width,
                message_height
                );
            
            graphics::Text text{screen, font, show_message_component.text, graphics::Color::BLACK, message_width};
            
            graphics::printTextScaled(
                screen,
                text,
                transform_component.position.x + 30.0f,
                transform_component.position.y - message_height + 10.0f,
                0.6f,
                0.6f
                );
        }
    }
private:
    entt::registry& registry;
};
