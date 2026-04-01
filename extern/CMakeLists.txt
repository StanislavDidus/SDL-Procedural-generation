
#SDL
set(SDL_SHARED OFF CACHE BOOL "Don't build SDL shared" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL static" FORCE)
add_subdirectory(SDL EXCLUDE_FROM_ALL)
#SDL_ttf
set(SDLTTF_VENDORED ON)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)
#SDL_shadercross
set(SDLSHADERCROSS_SHARED OFF CACHE BOOL "Don't build SDL_shadecross shared" FORCE)
set(SDLSHADERCROSS_STATIC ON CACHE BOOL "Build SDL_shadecross static" FORCE)
set(SDLSHADERCROSS_SPIRVCROSS_SHARED OFF CACHE BOOL "Build Spirvcross shared" FORCE)
set(SDLSHADERCROSS_DXC OFF CACHE BOOL "Disable DXC compilation")
set(SDLSHADERCROSS_VENDORED ON)
add_subdirectory(SDL_shadercross EXCLUDE_FROM_ALL)
#glm
add_subdirectory(glm EXCLUDE_FROM_ALL)
#tinyxml2
add_subdirectory(tinyxml2 EXCLUDE_FROM_ALL)
#entt
add_subdirectory(entt EXCLUDE_FROM_ALL)

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
    SDL3_ttf-static
    SDL3_shadercross
    SDL3_shadercross-static
    SDL3-static
    glm
    imgui
    tinyxml2
    freetype
    harfbuzz
    plutosvg
    plutovg
    SDL_uclibc
    SDL3_test
    EnTT
    copy_assets
    core_tables
    DxcDisassembler
    DxcOptimizer
    XcilIntrinsicTables
    DxilPIXPasses
    DxilValidation
    DxilIntrinsicTables
    DxilValidationInc
    extinst_tables
    HCTGen
    spirv-cross-c
    spirv-cross-core
    spirv-cross-cpp
    spirv-cross-glsl
    spirv-cross-hlsl
    spirv-cross-msl
    spirv-cross-reflect
    spirv-tools-tables
)

set_targets_folder("${ALL_LIBS}" "externals")