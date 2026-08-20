cmake_minimum_required(VERSION 3.28)
project(TexInvertPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(TexInvertPlugin SHARED
    TexInvertPlugin.cpp
)

target_include_directories(TexInvertPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_compile_definitions(TexInvertPlugin PRIVATE
    NF_PLUGIN_BUILD_EXPORT
)

set_target_properties(TexInvertPlugin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "TexInvertPlugin"
)
