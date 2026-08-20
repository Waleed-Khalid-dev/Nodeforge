cmake_minimum_required(VERSION 3.28)
project(DataCSVTransformPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DataCSVTransformPlugin SHARED
    DataCSVTransformPlugin.cpp
)

target_include_directories(DataCSVTransformPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_compile_definitions(DataCSVTransformPlugin PRIVATE
    NF_PLUGIN_BUILD_EXPORT
)

set_target_properties(DataCSVTransformPlugin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "DataCSVTransformPlugin"
)
