
#SDL
add_subdirectory(SDL EXCLUDE_FROM_ALL)
#SDL_ttf
set(SDLTTF_VENDORED ON)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)
#glm
add_subdirectory(glm EXCLUDE_FROM_ALL)
#tinyxml2
add_subdirectory(tinyxml2 EXCLUDE_FROM_ALL)

#ImGui
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

target_link_libraries(imgui PUBLIC SDL3::SDL3-static)

macro( set_targets_folder TARGETS FOLDER )
	foreach( target ${TARGETS} )
		if( TARGET ${target} )
			set_target_properties( ${target} PROPERTIES FOLDER ${FOLDER})
		endif()
	endforeach()
endmacro()

set ( ALL_LIBS
    SDL3-static
    SDL3_ttf
    glm
    imgui
    tinyxml2
    freetype
    harfbuzz
    plutosvg
    plutovg
    SDL_uclibc
    SDL3_test
    SDL3_ttf-static
)

set_targets_folder("${ALL_LIBS}" "externals")