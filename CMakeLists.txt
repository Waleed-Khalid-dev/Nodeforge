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

# Enable GTest for Phase 1 Smoke Test
find_package(GTest         CONFIG REQUIRED)

# Phase 2+: uncomment as phases unlock
# find_package(benchmark     CONFIG REQUIRED)
# find_package(pybind11      CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
# find_package(OpenCV        CONFIG REQUIRED)
# find_package(assimp        CONFIG REQUIRED)

# ─── Main executable ────────────────────────────────────────────────────────
add_executable(nodeforge
    src/app/main.cpp
    src/gpu/Device.cpp
    src/gpu/Swapchain.cpp
    src/gpu/Texture.cpp
    src/gpu/FrameResources.cpp
)

target_include_directories(nodeforge PRIVATE
    ${Stb_INCLUDE_DIR}
)

target_link_libraries(nodeforge PRIVATE
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

# Warnings as errors on core (Phase 0 rule)
target_compile_options(nodeforge PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /wd4996>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wpedantic>
)

target_compile_definitions(nodeforge PRIVATE
    NF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NF_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NF_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    $<$<CONFIG:Debug>:NF_DEBUG=1>
)

# ─── Tests (unlock in Phase 2) ──────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)

# ─── Phase tracking note ────────────────────────────────────────────────────
# Phase 0: only main.cpp — GLFW window opens and closes. No operators.
# Phase 1: add src/gpu/ — Vulkan device, swapchain, texture upload.
# Do NOT add Phase 1+ code until STATUS.md says Active phase: 1.
