cmake_minimum_required(VERSION 3.15...4.2.0)

project(SDL_PRO_GEN VERSION 1.0
    DESCRIPTION "Small SDL procedural generation game"
    LANGUAGES CXX)

add_executable(test src/main.cpp inc/header.hpp)

target_include_directories(test PUBLIC inc)
target_compile_features(test PUBLIC cxx_std_11)