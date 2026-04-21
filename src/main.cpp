#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_mouse.h>
#include <iostream>
#include <chrono>

#include "Color.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include "imgui_impl_sdlgpu3_shaders.h"

#include "Window.hpp"
#include "GpuRenderer.hpp"
#include "Game.hpp"

#include "InputManager.hpp"

#include <entt/entt.hpp>

// Sounds
#include "AudioDevice.hpp"
#include "Sound.hpp"

#include "GpuRenderFunctions.hpp"
#include "SDL3_shadercross/SDL_shadercross.h"

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

using namespace graphics;

static float angle = 0.0f;

int main()
 {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        std::cerr << "ERROR: Could not initialize SDL: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }
    
    if (!TTF_Init())
    {
        std::cerr << "ERROR: Could not initialize TTF: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }
    if (!SDL_ShaderCross_Init())
    {
	    
        std::cerr << "ERROR: Could not initialize SDL_ShaderCross: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    //Initialize randomizer
    srand(time(0));

    try
    {
        /*
        audio::AudioDevice audio_device{};
		audio::Sound sound{audio_device.getAudioDeviceID(), "assets/Sounds/sample.wav"};
		audio::Sound sound1{audio_device.getAudioDeviceID(), "assets/Sounds/item_collected.wav"};
        sound.setVolume(1.0f);
		sound.play();
        sound.play();
        sound.play();
		sound1.setVolume(1.0f);
		*/
		//sound1.play();

        Window window{ "First SDL program", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE };
        //graphics::GpuRenderer renderer{ window };
        GpuRenderer gpu_renderer{ window };
		ResourceManager::get().loadXml("data/assets.xml", gpu_renderer);
        //auto car_texture = gpu_renderer.loadTexture(Surface{"assets/Sprites/car.bmp"});
        //auto ice_cream_texture = gpu_renderer.loadTexture(Surface{ ResourceManager::get().getFont("Main"), "Debug", Color::RED});

        //Sprite sprite{ car_texture, SDL_FRect{0.0f, 0.0f, 5184.0f, 3456.0f} };
        Game game{ gpu_renderer };

        InputManager input_manager;

        //auto sprite1 = ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("Sky");

        //Init ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLGPU(window.get());
        ImGui_ImplSDLGPU3_InitInfo init_info = {};
        init_info.Device = gpu_renderer.getDevice().get();
        init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_renderer.getDevice().get(), window.get());
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
        init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
        init_info.PresentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
    	ImGui_ImplSDLGPU3_Init(&init_info);
        ImGui::GetIO().FontGlobalScale = 1.5f;

    	Text debug_test{ gpu_renderer, ResourceManager::get().getFont("Main"), "Debug", Color::BLUE };
        float dt = 0.f;
        while (window)
        {
            const auto start = std::chrono::steady_clock::now();

            //Do everything here
            SDL_Event event;
            while (window.pollEvent(event))
            {
                ImGui_ImplSDL3_ProcessEvent(&event);

                //Handle input
                switch (event.type)
                {
                case SDL_EVENT_KEY_DOWN:
                    input_manager.buttonPressed(event.key.scancode);
                    break;
                case SDL_EVENT_KEY_UP:
                    input_manager.buttonUp(event.key.scancode);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    input_manager.setMouseWheel({ event.wheel.x, event.wheel.y });
                    break;
                }
            }

            dt = std::min(dt, 0.033f);

            //Update mouse input
            float mouse_x, mouse_y = 0.f;
            SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
            //SDL_RenderCoordinatesFromWindow(renderer.get(), mouse_x, mouse_y, &mouse_x, &mouse_y);

            input_manager.setMouseState(
                { mouse_x, mouse_y },
                static_cast<bool>(buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)),
                static_cast<bool>(buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT))
            );

            //clear(renderer, Color::BLACK);

            input_manager.update();

            game.tick(dt);

            //printText(gpu_renderer, debug_test, 0.0f, 0.0f, 200.0f, 200.0f, false);

            //gpu_renderer.renderTexture(ice_cream_texture, std::nullopt, std::nullopt, 0.0f, SDL_FLIP_NONE);

            //gpu_renderer.renderTriangle(0.0f, 0.0f, 480.0f, 540.0f, 960.0f, 0.0f, SDL_FColor{ 1.0f, 0.0,0.0f, 1.0f });
            //graphics::drawScaledSprite(gpu_renderer, sprite, 100.0f, 100.0f, 500.0f, 300.0f, IGNORE_VIEW_ZOOM);
            //gpu_renderer.renderSprite(ResourceManager::get().getSpriteSheet("backgrounds")->getSprite("Sky"), 0.0f, 0.0f,200.0f, 200.0f, angle);
            //gpu_renderer.renderSprite(ResourceManager::get().getSpriteSheet("objects")->getSprite("Tree"), 300.0f, 0.0f,200.0f, 200.0f, angle);
            //gpu_renderer.renderRectangle(1.0f, 1.0f, 100.0f, 100.0f, RenderType::NONE, Color::YELLOW, IGNORE_VIEW_ZOOM);
            //gpu_renderer.renderRectangle(200.0f, 200.0f, 100.0f, 100.0f, RenderType::FILL, Color::BLUE, IGNORE_VIEW_ZOOM);
            //gpu_renderer.renderSprite(sprite, 0.0f, 0.0f, 100.0f, 100.0f, angle);
            //gpu_renderer.renderSprite(car_texture, 300.0f, 300.0f,200.0f, 200.0f, angle);
            //gpu_renderer.renderSprite("Ice-cream", 300.0f, 200.0f, 200.0f, 200.0f);
            //gpu(100.0f, 100.0f, 200.0f, 200.0f, SDL_FColor{ 1.0f, 0.0f, 0.0f, 1.0f });
            //drawRectangle(gpu_renderer, 100.0f, 100.0f, 200.0f, 200.0f, RenderType::FILL, Color::RED);
            gpu_renderer.update();

            //update(renderer);

            // Reset mousewheel state
            input_manager.setMouseWheel(glm::vec2{ 0.0f });

            const auto finish = std::chrono::steady_clock::now();
            const std::chrono::duration<double> elapsed_seconds{ finish - start };
            dt = static_cast<float>(elapsed_seconds.count());
        }

        //Destroy imgui
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();
    }
    catch (const std::exception& e)
    {
        std::cout << "Unexpected error occured: " << e.what() << std::endl;
        throw;
    }
}
