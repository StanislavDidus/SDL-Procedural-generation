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
        for (auto& message : messages)
        {
            registry.get<Components::UI::ShowMessage>(message).timer += dt;
            break;     
        }
        
        messages.erase(std::ranges::remove_if(messages, [&](const Entity& entity)
        {
            const auto& show_message_component = registry.get<Components::UI::ShowMessage>(entity);
            if (show_message_component.timer >= show_message_component.time)
            {
                registry.destroy(entity);
                return true;
            }
            return false;
        }).begin(), messages.end());
    }
    
    void render(graphics::GpuRenderer& screen) const
    {
        const auto& sprite = ResourceManager::get().getSpriteSheet("ui")->getSprite("Dialogue_Window");
        const auto& font = ResourceManager::get().getFont("Main");
        float zoom = screen.getZoom();
        
        zoom = std::clamp(zoom, 0.8f, 2.0f);
        
        float message_width = 170.0f / zoom;
        float message_height = 150.0f / zoom;
        float text_offset_x = 30.0f / zoom;
        float test_offset_y = 10.0f / zoom;
        float text_scale = 0.5f / zoom;
        //graphics::zoomPoint(screen, message_width, message_height);
        
        for (const auto& message : messages)
        {
            const auto& show_message_component = registry.get<Components::UI::ShowMessage>(message);
            
            if (registry.all_of<Components::Transform>(show_message_component.target))
            {
                const auto& transform_component = registry.get<Components::Transform>(show_message_component.target);
                
                //const auto screen_position = graphics::getScreenPositionForWorld(screen, transform_component.position);
                
                graphics::drawScaledSprite(
                    screen,
                    sprite,
                    transform_component.position.x,
                    transform_component.position.y - message_height,
                    message_width,
                    message_height
                    );
            
                graphics::Text text{screen, font, show_message_component.text, graphics::Color::BLACK, 230.0f};
            
                graphics::printTextScaled(
                    screen,
                    text,
                    transform_component.position.x + text_offset_x,
                    transform_component.position.y - message_height + test_offset_y,
                    text_scale,
                    text_scale
                    );
            }
            
            break;
        }
    }
    
    void message(Entity target, const std::string& text, float duration)
    {
        Entity message = registry.create();
        registry.emplace<Components::UI::ShowMessage>(message, target, text, duration);
        messages.emplace_back(message);
    }
private:
    entt::registry& registry;
    std::vector<Entity> messages;
};
