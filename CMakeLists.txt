cmake_minimum_required(VERSION 3.15...4.2.1)

project(SDL_PRO_GEN VERSION 1.0
    DESCRIPTION "Small SDL procedural generation game"
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
    )

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(SDL_PRO_GEN ${ALL_FILES})

add_custom_command(
    TARGET SDL_PRO_GEN POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "${CMAKE_BINARY_DIR}/assets"
)

target_include_directories(SDL_PRO_GEN PUBLIC inc)

target_link_libraries(
    SDL_PRO_GEN PUBLIC
    SDL3::SDL3-static
    SDL3_ttf::SDL3_ttf
    graphics 
    glm::glm 
    noise
    imgui
)

target_compile_features(SDL_PRO_GEN PUBLIC cxx_std_20)