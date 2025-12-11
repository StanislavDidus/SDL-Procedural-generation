set(NOISE_SOURCE_FILES 
    "src/Noise.cpp"
    "src/ValueNoise.cpp"
    "src/PerlynNoise.cpp"
    )

set(NOISE_HEADER_FILES
    "inc/Noise.hpp"
    "inc/ValueNoise.hpp"
    "inc/PerlynNoise.hpp"
    )

set(NOISE_ALL_FILES ${NOISE_SOURCE_FILES} ${NOISE_HEADER_FILES})

add_library(noise STATIC ${NOISE_ALL_FILES})

target_include_directories(noise PUBLIC inc)

target_link_libraries(noise PUBLIC glm::glm)

target_compile_features(noise PUBLIC cxx_std_17)