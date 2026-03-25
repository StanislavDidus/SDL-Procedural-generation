cmake_minimum_required(VERSION 3.15...4.2.1)

project(SDL_PRO_GEN VERSION 1.0
    DESCRIPTION "Small survival game with procedural generation made with SDL3"
    LANGUAGES C CXX)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libs" FORCE)

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
    #"src/UI/Button.cpp"
    "src/ResourceManager.cpp"
    "src/EnemyManager.cpp"
    "src/EnemySpawnSystem.cpp"
    "src/DrunkWalker.cpp"
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
    #"inc/UI/Button.hpp"
    "inc/ResourceManager.hpp"
    "inc/ECS/RenderCraftingUISystem.hpp"
    "inc/ECS/ItemDescriptionSystem.hpp"
    "inc/ECS/Components.hpp"
    "inc/ECS/RenderSystem.hpp"
    "inc/UI/UISettings.hpp"
    "inc/ECS/RenderWeaponMenuSystem.hpp"
    "inc/EnemyManager.hpp"
    "inc/EnemySpawnSystem.hpp"
    "inc/ECS/EnemyAISystem.hpp"
    "inc/ECS/PlayerComboSystem.hpp"
    "inc/ECS/ApplyDamageSystem.hpp"
    "inc/ECS/DisplayHitMarksSystem.hpp"
    "inc/ECS/RenderWeaponCircle.hpp"
    "inc/ECS/DropItemSystem.hpp"
    "inc/ECS/InventoryManageSystem.hpp"
    "inc/ECS/EnttCopyComponents.hpp"
    "inc/ECS/ApplyArmorEffects.hpp"
    "inc/ECS/RenderHealthBarSystem.hpp"
    "inc/ECS/PlayerCollisionSystem.hpp"
    "inc/ECS/ManageInvincibleStatusSystem.hpp"
    "inc/ECS/DeathSystem.hpp"
    "inc/ECS/ChangeMiningSizeSystem.hpp"
    "inc/WorldHelper.hpp"
    "inc/ECS/ManageButtonActionsSystem.hpp"
    "inc/ECS/ChestWindowSystem.hpp"
    "inc/ECS/WindowManagerSystem.hpp"
    "inc/ECS/RenderEssenceCounter.hpp"
    "inc/ECS/CollectEssenceSystem.hpp"
    "inc/ECS/OpenChestSystem.hpp"
    "inc/ECS/DropChestLootSystem.hpp"
    "inc/DrunkWalker.hpp"
    "inc/ECS/HealthRegenerationSystem.hpp"
    "inc/ECS/UpdateEffects.hpp"
    "inc/ECS/ApplyEffects.hpp"
    "inc/ECS/RenderAccessoriesSystem.hpp"
    "inc/EnemySpawnManager.hpp"
    "inc/VertexBuffer.hpp"
    "inc/WorldOutput.hpp"
    "inc/WorldRenderer.hpp"
    )

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES})

set(DXC_PATH "C:/tools/VulkanSDK/1.4.341.1/Bin/dxc.exe")

set(VERTEX_SHADERS
    "OnlyPosition"
    "TextureQuad"
)

set(FRAGMENT_SHADERS
    "SolidColor"
    "Gradient"
    "TextureQuad"
)

# Compile all shaders to .spv format
set(SHADERS_SOURCE_PATH ${CMAKE_SOURCE_DIR}/shaders)
set(SHADERS_OUTPUT_PATH ${CMAKE_BINARY_DIR}/shaders/compiled)

set(VERTEX_OUTPUTS)
set(FRAGMENT_OUTPUTS)

foreach(SHADER IN LISTS VERTEX_SHADERS)
add_custom_command(
    OUTPUT ${SHADERS_OUTPUT_PATH}/${SHADER}.vert.spv
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_OUTPUT_PATH}
    COMMAND ${DXC_PATH} -spirv -T vs_6_0 -E main 
        ${SHADERS_SOURCE_PATH}/${SHADER}.vert.hlsl 
        -Fo ${SHADERS_OUTPUT_PATH}/${SHADER}.vert.spv 
    DEPENDS ${SHADERS_SOURCE_PATH}/${SHADER}.vert.hlsl
)
list(APPEND VERTEX_OUTPUTS ${SHADERS_OUTPUT_PATH}/${SHADER}.vert.spv)
endforeach()

foreach(SHADER IN LISTS FRAGMENT_SHADERS)
add_custom_command(
    OUTPUT ${SHADERS_OUTPUT_PATH}/${SHADER}.frag.spv
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_OUTPUT_PATH}
    COMMAND ${DXC_PATH} -spirv -T ps_6_0 -E main 
        ${SHADERS_SOURCE_PATH}/${SHADER}.frag.hlsl 
        -Fo ${SHADERS_OUTPUT_PATH}/${SHADER}.frag.spv 
    DEPENDS ${SHADERS_SOURCE_PATH}/${SHADER}.frag.hlsl
)
list(APPEND FRAGMENT_OUTPUTS ${SHADERS_OUTPUT_PATH}/${SHADER}.frag.spv)
endforeach()

add_custom_target(compile_shaders ALL
    DEPENDS 
        ${VERTEX_OUTPUTS}
        ${FRAGMENT_OUTPUTS}
)

add_executable(SDL_PRO_GEN ${ALL_FILES})

add_dependencies(SDL_PRO_GEN compile_shaders)

if(MSVC)
    target_compile_options(SDL_PRO_GEN PRIVATE /bigobj)
endif()

#Copy assets and data folders to the build folder
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_assets.cmake
)
add_dependencies(SDL_PRO_GEN copy_assets)

target_include_directories(SDL_PRO_GEN PUBLIC inc)

target_link_libraries(
    SDL_PRO_GEN PUBLIC
    SDL3_ttf::SDL3_ttf-static
    SDL3::SDL3-static
    graphics 
    glm::glm 
    noise
    imgui
    tinyxml2
    EnTT::EnTT
)

target_compile_features(SDL_PRO_GEN PUBLIC cxx_std_23)