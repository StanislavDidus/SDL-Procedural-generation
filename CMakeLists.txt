cmake_minimum_required(VERSION 3.15...4.2.1)

project(SDL_PRO_GEN VERSION 1.0
    DESCRIPTION "Small survival game with procedural generation made with SDL3"
    LANGUAGES C CXX)

add_subdirectory(EXTERN extern)
add_subdirectory(GRAPHICS graphics)
add_subdirectory(NOISE noise)

set(SOURCE_FILES 
    "src/Game.cpp"
    "src/main.cpp" 
    "src/Biomes.cpp"
    "src/TileMap.cpp"
    "src/Tile.cpp"
    "src/World.cpp"
    "src/Chunk.cpp"
    "src/Player.cpp"
    "src/MapRange.cpp"
    "src/ECS/Entity.cpp"
    "src/ECS/EntityManager.cpp"
    "src/ECS/ComponentManager.cpp"
    "src/InputManager.cpp"
    "src/CollisionManager.cpp"
    "src/Inventory.cpp"
    "src/Item.cpp"
    "src/UI/UserInterface.cpp"
    "src/UI/FillBar.cpp"
    "src/UI/InventoryView.cpp"
    "src/ItemManager.cpp"
    "src/ObjectManager.cpp"
    "src/TileManager.cpp"
    "src/Object.cpp"
    "src/Craft.cpp"
    "src/CraftView.cpp"
    "src/CraftingManager.cpp"
    "src/Manager.cpp"
    "src/UI/Button.cpp"
    "src/ResourceManager.cpp"
    )

set(HEADER_FILES
    "inc/Game.hpp"
    "inc/Biomes.hpp"
    "inc/TileMap.hpp"
    "inc/Tile.hpp"
    "inc/World.hpp"
    "inc/Chunk.hpp"
    "inc/Player.hpp"
    "inc/MapRange.hpp"
    "inc/ECS/Entity.hpp"
    "inc/ECS/EntityManager.hpp"
    "inc/ECS/ComponentManager.hpp"
    "inc/ECS/Systems.hpp"
    "inc/InputManager.hpp"
    "inc/CollisionManager.hpp"
    "inc/Debug.hpp"
    "inc/Inventory.hpp"
    "inc/Item.hpp"
    "inc/UI/UserInterface.hpp"
    "inc/UI/UIElement.hpp"
    "inc/UI/FillBar.hpp"
    "inc/UI/InventoryView.hpp"
    "inc/ItemManager.hpp"
    "inc/Object.hpp"
    "inc/ObjectManager.hpp"
    "inc/Vec2Less.hpp"
    "inc/Grid.hpp"
    "inc/TileManager.hpp"
    "inc/GenerationData.hpp"
    "inc/RandomizedItem.hpp"
    "inc/Craft.hpp"
    "inc/CraftView.hpp"
    "inc/CraftingManager.hpp"
    "inc/Manager.hpp"
    "inc/UI/Button.hpp"
    "inc/ResourceManager.hpp"
    "inc/ECS/RenderUISystem.hpp"
    "inc/ECS/ItemDescriptionSystem.hpp"
    )

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(SDL_PRO_GEN ${ALL_FILES})

#Copy assets and data folders to the build folder
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_assets.cmake
)
add_dependencies(SDL_PRO_GEN copy_assets)

target_include_directories(SDL_PRO_GEN PUBLIC inc)

target_link_libraries(
    SDL_PRO_GEN PUBLIC
    SDL3::SDL3-static
    SDL3_ttf::SDL3_ttf
    graphics 
    glm::glm 
    noise
    imgui
    tinyxml2
)

target_compile_features(SDL_PRO_GEN PUBLIC cxx_std_20)