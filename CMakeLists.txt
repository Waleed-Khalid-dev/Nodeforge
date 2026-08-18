cmake_minimum_required(VERSION 3.28)
project(NodeForge VERSION 0.1.0 LANGUAGES CXX)

# ─── C++ Standard ───────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Build type default ─────────────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "NodeForge ${PROJECT_VERSION} | ${CMAKE_BUILD_TYPE} | ${CMAKE_CXX_COMPILER_ID}")

# ─── Output directories ─────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ─── Dependencies (via vcpkg toolchain) ─────────────────────────────────────
find_package(glfw3         CONFIG REQUIRED)
find_package(glm           CONFIG REQUIRED)
find_package(imgui         CONFIG REQUIRED)
find_package(spdlog        CONFIG REQUIRED)
find_package(fmt           CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan        REQUIRED)
find_package(vk-bootstrap  CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(Stb           REQUIRED)
find_package(GTest         CONFIG REQUIRED)
find_package(benchmark     CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

# ─── Core Library ───────────────────────────────────────────────────────────
add_library(nodeforge_core STATIC
    src/gpu/Device.cpp
    src/gpu/Swapchain.cpp
    src/gpu/Texture.cpp
    src/gpu/FrameResources.cpp
    src/graph/PinValue.cpp
    src/graph/Pin.cpp
    src/graph/Wire.cpp
    src/graph/Node.cpp
    src/graph/Graph.cpp
    src/graph/GraphSerializer.cpp
    src/graph/NodeRegistry.cpp
    src/graph/CoreNodes.cpp
    src/operators/chan/ConstantChanOp.cpp
    src/operators/chan/MathChanOp.cpp
    src/operators/tex/ConstantTexOp.cpp
    src/operators/tex/TransformTexOp.cpp
)

target_include_directories(nodeforge_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Stb_INCLUDE_DIR}
)

target_link_libraries(nodeforge_core PUBLIC
    glfw
    glm::glm
    imgui::imgui
    spdlog::spdlog
    fmt::fmt
    nlohmann_json::nlohmann_json
    Vulkan::Vulkan
    vk-bootstrap::vk-bootstrap
    GPUOpen::VulkanMemoryAllocator
    unofficial::shaderc::shaderc
)

target_compile_options(nodeforge_core PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /wd4996 /wd4251 /wd4275>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wpedantic>
)

target_compile_definitions(nodeforge_core PUBLIC
    NF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NF_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NF_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    $<$<CONFIG:Debug>:NF_DEBUG=1>
)

# ─── Main executable ────────────────────────────────────────────────────────
add_executable(nodeforge
    src/app/main.cpp
)

target_link_libraries(nodeforge PRIVATE
    nodeforge_core
)

# ─── Tests & Benchmarks ─────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)
