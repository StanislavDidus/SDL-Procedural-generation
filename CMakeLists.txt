cmake_minimum_required(VERSION 3.15...4.2.0)

project(SDL_PRO_GEN VERSION 1.0
    DESCRIPTION "Small SDL procedural generation game"
    LANGUAGES C CXX)

add_subdirectory(EXTERN extern)
add_subdirectory(GRAPHICS graphics)

set(SOURCE_FILES 
    "src/Game.cpp"
    "src/main.cpp" 
    )

set(HEADER_FILES
    "inc/Game.hpp"
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

target_link_libraries(SDL_PRO_GEN PUBLIC SDL3::SDL3-static graphics glm::glm)

target_compile_features(SDL_PRO_GEN PUBLIC cxx_std_17)