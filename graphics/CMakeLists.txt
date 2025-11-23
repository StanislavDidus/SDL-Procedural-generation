set(GRAPHICS_SOURCE_FILES 
    "src/Renderer.cpp"
    "src/Sprite.cpp"
    "src/Surface.cpp"
    "src/Window.cpp"
    )

set(GRAPHICS_HEADER_FILES
    "inc/Renderer.hpp"
    "inc/Sprite.hpp"
    "inc/Surface.hpp"
    "inc/Window.hpp"
    )

set(GRAPHICS_ALL_FILES ${GRAPHICS_SOURCE_FILES} ${GRAPHICS_HEADER_FILES})

add_library(graphics STATIC ${GRAPHICS_ALL_FILES})

target_include_directories(graphics PUBLIC inc)

target_link_libraries(graphics PUBLIC SDL3::SDL3-static glm::glm)

target_compile_features(graphics PUBLIC cxx_std_17)