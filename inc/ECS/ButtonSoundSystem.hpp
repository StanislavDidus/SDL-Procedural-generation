#pragma once
#include <entt/entity/registry.hpp>

#include "Components.hpp"

class ButtonSoundSystem
{
public:
    ButtonSoundSystem(entt::registry& registry)
        : registry{registry} {}
    ~ButtonSoundSystem() = default;
    
    void update()
    {
        // Button Entered Sound
        auto view = registry.view<Components::ButtonEntered, Components::ButtonEnteredSound>();
        for (auto [entity, button_entered_sound] : view.each())
        {
            button_entered_sound.sound->play(); 
        }
        
        // Button Released Sound
        auto view1 = registry.view<Components::ButtonReleased, Components::ButtonReleasedSound>();
        for (auto [entity, button_released_sound] : view1.each())
        {
            button_released_sound.sound->play();
            std::cout << "released" << std::endl;
        }            
    }
private:
    entt::registry& registry;
};
