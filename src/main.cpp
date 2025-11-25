#include <SDL3/SDL.h>
#include <iostream>
#include <chrono>

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

    Window window{ "First SDL program", 960, 540, 0 };
    Renderer renderer{ window };
    Game game{ renderer };

    float dt = 0.f;
    bool running = true;
    while (running)
    {
        const auto start = std::chrono::steady_clock::now();

        //Do everything here
        SDL_Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_Q:
                    game.scale += 0.001f;
                    break;
                case SDLK_E:
                    game.scale -= 0.001f;
                    break;
                case SDLK_SPACE:
                    game.generateSeed();
                    break;
                //Camera control
                case SDLK_W:
                    game.view_position.y -= 20.f;
                    break;
                case SDLK_S:
                    game.view_position.y += 20.f;
                    break;
                case SDLK_A:
                    game.view_position.x -= 20.f;
                    break;
                case SDLK_D:
                    game.view_position.x += 20.f;
                    break;
                }
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

    return 0;
}
