set(GRAPHICS_SOURCE_FILES 
    "src/Renderer.cpp"
    "src/Sprite.cpp"
    "src/Surface.cpp"
    "src/Window.cpp"
    "src/SpriteSheet.cpp" 
    "src/Color.cpp"
    "src/Font.cpp"
    "src/Text.cpp"
    "src/ViewportGuard.cpp"
    )

set(GRAPHICS_HEADER_FILES
    "inc/Renderer.hpp"
    "inc/Sprite.hpp"
    "inc/Surface.hpp"
    "inc/Window.hpp"
    "inc/SpriteSheet.hpp"
    "inc/Color.hpp"
    "inc/Font.hpp"
    "inc/Text.hpp"
    "inc/ViewportGuard.hpp"
    "inc/RenderFunctions.hpp"
    )

set(GRAPHICS_ALL_FILES ${GRAPHICS_SOURCE_FILES} ${GRAPHICS_HEADER_FILES})

add_library(graphics STATIC ${GRAPHICS_ALL_FILES})

target_include_directories(graphics PUBLIC inc)

target_link_libraries(graphics PUBLIC SDL3::SDL3-static SDL3_ttf::SDL3_ttf-static glm::glm)

target_compile_features(graphics PUBLIC cxx_std_23)