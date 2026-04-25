#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"

class ButtonSpriteSystem
{
public:
    ButtonSpriteSystem(entt::registry& registry)
        : registry{registry} {}
    ~ButtonSpriteSystem() = default;
    
    void update()
    {
        // Button Entered Sprite
        auto view = registry.view<Components::ButtonEntered, Components::Renderable, Components::ButtonEnteredSprite>();
        for (auto [entity, renderable, button_entered_sprite] : view.each())
        {
            renderable.sprite = button_entered_sprite.sprite;
        }
        
        // Button Exit Sprite
        auto view1 = registry.view<Components::ButtonExit, Components::Renderable, Components::ButtonExitSprite>();
        for (auto [entity, renderable, button_exit_sprite] : view1.each())
        {
            renderable.sprite = button_exit_sprite.sprite; 
        }            
    }
private:
    entt::registry& registry;
};
