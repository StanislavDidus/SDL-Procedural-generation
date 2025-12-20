#include <SDL3/SDL.h>
#include <iostream>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "Window.hpp"
#include "Renderer.hpp"
#include "Game.hpp"

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "ERROR: Could not initialize SDL: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    Window window{ "First SDL program", 960, 540, SDL_WINDOW_RESIZABLE};
    Renderer renderer{ window };
    Game game{ renderer };

    //Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window.getWindow(), renderer.getRenderer() );
    ImGui_ImplSDLRenderer3_Init(renderer.getRenderer());
    //ImGui::GetIO().FontGlobalScale = 1.5f;

    float dt = 0.f;
    while (window)
    {
        const auto start = std::chrono::steady_clock::now();

        //Do everything here
        SDL_Event event;
        while (window.pollEvent(event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
                game.buttonPressed(event.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                game.buttonUp(event.key.key);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                game.resizeSprites();
                break;
            }
        }

        renderer.clear(Color::BLACK);

        game.update(dt);

        renderer.update(dt);

        const auto finish = std::chrono::steady_clock::now();   
        const std::chrono::duration<double> elapsed_seconds{ finish - start };
        dt = static_cast<float>(elapsed_seconds.count());
    }
    
    //Destroy imgui
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
