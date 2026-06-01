#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_mouse.h>
#include <iostream>
#include <chrono>

#include <graphics/Color.hpp>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include "imgui_impl_sdlgpu3_shaders.h"

#include <graphics/Window.hpp>
#include <graphics/Renderer.hpp>
#include "Game.hpp"

#include "InputManager.hpp"

#include <entt/entt.hpp>

// Sounds
#include "AudioDevice.hpp"
#include "Sound.hpp"

#include "GpuRenderFunctions.hpp"
#include "graphics/graphics.hpp"

#ifdef __EMSCRIPTEN__
    #include "emscripten.h"
#endif 

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

using namespace graphics;

namespace graphics
{
    int MAX_SPRITES_RENDERED = 2000;
    int MAX_RECTANGLES_RENDERED = 500;
    int MAX_LINES_RENDERED = 100;
    int MAX_TILEMAPS_RENDERED = 100;
}

struct context
{
    std::unique_ptr<Window> window;
    std::unique_ptr<graphics::Renderer> renderer;
    std::unique_ptr<Game> game;
    std::unique_ptr<InputManager> input_manager;
    float dt = 0.0f;
    float ems_dt = 0.0f;
    float ems_last_time = 0.0f;
};

void initialize_game(context& ctx)
{
    // For using RenderDoc
    //SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    //SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
    
    graphics::init(); 
    
    //Initialize randomizer
    srand(time(0));
    
    ctx.window = std::make_unique<Window>("RaTe-02", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE); 
    ctx.renderer = std::make_unique<graphics::Renderer>(*ctx.window);
    
    // Set Window Icon
    Surface icon_surface{"assets/Sprites/icon.png"};
    SDL_SetWindowIcon(ctx.window->get(), icon_surface.get());
    
    // Initialize sprite assets
    std::cout << "Trying to load assets." << std::endl;
    ResourceManager::get().loadXml("data/assets.xml", *ctx.renderer);
    std::cout << "Successfully loaded assets." << std::endl;
    
    // Create Game
    ctx.game = std::make_unique<Game>(*ctx.renderer);

    ctx.input_manager = std::make_unique<InputManager>();
}

void deinitialize_game(context& ctx)
{
}

void update_game(void* arg)
{
    context* ctx = static_cast<context*>(arg);
    
    if (!ctx->window->isOpen())
    {
        deinitialize_game(*ctx);
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#else
        std::exit(0);
#endif
    }       
    
    double start = 0.0;
#ifndef __EMSCRIPTEN__
    start = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
#else
    start = emscripten_get_now() / 1000.0;
#endif

#ifdef __EMSCRIPTEN__
    ctx->ems_dt = std::min(ctx->ems_dt, 0.033f);
    ctx->ems_dt = static_cast<float>(start - ctx->ems_last_time);
    ctx->ems_last_time = start;
#else
    ctx->dt = std::min(ctx->dt, 0.033f);
#endif
    
    SDL_Event event{};
    while (ctx->window->pollEvent(event))
    {
        //ImGui_ImplSDL3_ProcessEvent(&event);

        //Handle input
        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
            ctx->input_manager->buttonPressed(event.key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            ctx->input_manager->buttonUp(event.key.scancode);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            ctx->input_manager->setMouseWheel({ event.wheel.x, event.wheel.y });
            break;
        default: ;
        }
    }


    //Update mouse input
    float mouse_x, mouse_y = 0.f;
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    glm::vec2 mouse_position = ctx->renderer->getMouseScaledPosition({mouse_x, mouse_y});

    ctx->input_manager->setMouseState(
         mouse_position,
        static_cast<bool>(buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)),
        static_cast<bool>(buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT))
    );

    ctx->input_manager->update();

    ctx->renderer->startDrawing();
#ifdef __EMSCRIPTEN__
    ctx->game->tick(ctx->ems_dt);
#else
    ctx->game->tick(ctx->dt);
#endif
    ctx->renderer->endDrawing();
    
    ResourceManager::get().updateSounds();

    // Reset mousewheel state
    ctx->input_manager->setMouseWheel(glm::vec2{ 0.0f });
    
#ifndef __EMSCRIPTEN__
    double end = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    ctx->dt = end - start;
#endif
}

int main()
 {
    context ctx;
    initialize_game(ctx);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(update_game, &ctx, 0, true);
#else
    while (true)
    {
        try
        {
            update_game(&ctx);
        }
        catch (std::exception& e)
        {
            std::cout << "Exception caught: " << e.what() << std::endl;
        }
    }
#endif
}

