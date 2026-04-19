set(AUDIO_SOURCE_FILES
    "src/Sound.cpp"
    "src/AudioDevice.cpp"
)

set(AUDIO_HEADER_FILES
    "inc/Sound.hpp"
    "inc/AudioDevice.hpp"
)

set(AUDIO_ALL_FILES ${AUDIO_SOURCE_FILES} ${AUDIO_HEADER_FILES})

add_library(audio STATIC ${AUDIO_ALL_FILES})

target_include_directories(audio PUBLIC inc)
target_link_libraries(audio PUBLIC SDL3::SDL3-static)
target_compile_features(audio PUBLIC cxx_std_23)
