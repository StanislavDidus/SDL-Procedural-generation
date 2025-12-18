add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(glm EXCLUDE_FROM_ALL)

add_library(imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC imgui imgui/backends)

target_link_libraries(imgui PRIVATE SDL3::SDL3-static)
