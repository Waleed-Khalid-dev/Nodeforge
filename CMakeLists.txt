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
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# ─── Core Library ───────────────────────────────────────────────────────────
add_library(nodeforge_core STATIC
    src/gpu/Device.cpp
    src/gpu/Swapchain.cpp
    src/gpu/Texture.cpp
    src/gpu/TexturePool.cpp
    src/gpu/ShaderCompiler.cpp
    src/gpu/FrameResources.cpp
    src/render/FullscreenPass.cpp
    src/render/ComputePass.cpp
    src/core/ChannelBuffer.cpp
    src/core/DataTable.cpp
    src/core/GeometryData.cpp
    src/graph/PinValue.cpp
    src/graph/Pin.cpp
    src/graph/Wire.cpp
    src/graph/Node.cpp
    src/graph/Graph.cpp
    src/graph/GraphSerializer.cpp
    src/graph/NodeRegistry.cpp
    src/graph/CoreNodes.cpp
    src/param/Parameter.cpp
    src/param/ParameterGroup.cpp
    src/python/PythonEngine.cpp
    src/python/PyNodeForge.cpp
    src/operators/chan/ChanOp.cpp
    src/operators/chan/ConstantChanOp.cpp
    src/operators/chan/TimeChanOp.cpp
    src/operators/chan/LFOChanOp.cpp
    src/operators/chan/NoiseChanOp.cpp
    src/operators/chan/MathChanOp.cpp
    src/operators/chan/FilterChanOp.cpp
    src/operators/chan/MergeChanOp.cpp
    src/operators/chan/SelectChanOp.cpp
    src/operators/chan/TrailChanOp.cpp
    src/operators/chan/AudioFileInChanOp.cpp
    src/operators/chan/ChanToTexOp.cpp
    src/operators/chan/TexToChanOp.cpp
    src/operators/data/DataOp.cpp
    src/operators/data/TextDataOp.cpp
    src/operators/data/TableDataOp.cpp
    src/operators/data/ScriptDataOp.cpp
    src/operators/data/JSONDataOp.cpp
    src/operators/data/SelectDataOp.cpp
    src/operators/data/MergeDataOp.cpp
    src/operators/data/OSCInOp.cpp
    src/operators/data/OSCOutOp.cpp
    src/operators/data/WebDataOp.cpp
    src/operators/data/ChanToDataOp.cpp
    src/operators/data/DataToChanOp.cpp
    src/operators/geom/GeomOp.cpp
    src/operators/geom/GridGeomOp.cpp
    src/operators/geom/SphereGeomOp.cpp
    src/operators/geom/BoxGeomOp.cpp
    src/operators/geom/TorusGeomOp.cpp
    src/operators/geom/CylinderGeomOp.cpp
    src/operators/geom/TransformGeomOp.cpp
    src/operators/geom/MergeGeomOp.cpp
    src/operators/geom/NoiseDeformGeomOp.cpp
    src/operators/geom/NormalsGeomOp.cpp
    src/operators/geom/ChanToGeomOp.cpp
    src/operators/mat/MatOp.cpp
    src/operators/mat/ConstantMatOp.cpp
    src/operators/mat/PhongMatOp.cpp
    src/operators/mat/GLSLMatOp.cpp
    src/operators/comp/CameraComp.cpp
    src/operators/comp/LightComp.cpp
    src/operators/comp/GeometryComp.cpp
    src/media/VideoDecoder.cpp
    src/media/DisplayManager.cpp
    src/media/OutputWindow.cpp
    src/media/WarpMesh.cpp
    src/render/Scene3DPass.cpp
    src/render/WarpBlendPass.cpp
    src/io/midi/MidiManager.cpp
    src/io/serial/SerialPort.cpp
    src/io/dmx/ArtNetEngine.cpp
    src/io/input/InputManager.cpp
    src/operators/chan/MIDIInChanOp.cpp
    src/operators/chan/MIDIOutChanOp.cpp
    src/operators/chan/OSCInChanOp.cpp
    src/operators/chan/OSCOutChanOp.cpp
    src/operators/data/SerialDataOp.cpp
    src/operators/chan/DMXInChanOp.cpp
    src/operators/chan/DMXOutChanOp.cpp
    src/operators/chan/MouseInChanOp.cpp
    src/operators/chan/KeyboardInChanOp.cpp
    src/operators/tex/TexOp.cpp
    src/operators/tex/NullTexOp.cpp
    src/operators/tex/ConstantTexOp.cpp
    src/operators/tex/NoiseTexOp.cpp
    src/operators/tex/LoadImageTexOp.cpp
    src/operators/tex/TransformTexOp.cpp
    src/operators/tex/CompositeTexOp.cpp
    src/operators/tex/BlurTexOp.cpp
    src/operators/tex/LevelTexOp.cpp
    src/operators/tex/ResolutionTexOp.cpp
    src/operators/tex/ToWindowTexOp.cpp
    src/operators/tex/RenderTexOp.cpp
    src/operators/tex/MovieFileInTexOp.cpp
    src/operators/tex/VideoDeviceInTexOp.cpp
    src/operators/tex/SpoutInTexOp.cpp
    src/operators/tex/SpoutOutTexOp.cpp
    src/operators/tex/NDIInTexOp.cpp
    src/operators/tex/NDIOutTexOp.cpp
    src/operators/tex/ProjectorOutTexOp.cpp
    src/operators/tex/WarpBlendTexOp.cpp
    src/operators/comp/ContainerComp.cpp
    src/operators/comp/InOp.cpp
    src/operators/comp/OutOp.cpp
    src/project/PathUtils.cpp
    src/project/ProjectFile.cpp
    src/project/ProjectSerializer.cpp
    src/project/ComponentSerializer.cpp
    src/project/AutosaveManager.cpp
    src/project/RecentProjectsManager.cpp
    third_party/imgui_backends/imgui_impl_glfw.cpp
    third_party/imgui_backends/imgui_impl_vulkan.cpp
    src/ui/EditorTheme.cpp
    src/ui/EditorContext.cpp
    src/ui/UndoManager.cpp
    src/ui/commands/NodeCommands.cpp
    src/ui/commands/WireCommands.cpp
    src/ui/commands/ParamCommands.cpp
    src/ui/canvas/NodeCanvas.cpp
    src/ui/canvas/BreadcrumbBar.cpp
    src/ui/panels/OpPaletteModal.cpp
    src/ui/panels/ParameterPanel.cpp
    src/ui/panels/ViewerPanel.cpp
    src/ui/panels/ConsolePanel.cpp
    src/ui/panels/TimelinePanel.cpp
    src/ui/panels/ProfilerPanel.cpp
    src/ui/panels/PerformanceHUD.cpp
    src/ui/MainMenuBar.cpp
    src/profiling/CookProfiler.cpp
    src/gpu/GpuTimerPool.cpp
    src/diagnostics/CrashReporter.cpp
    src/plugin/PluginModule.cpp
    src/plugin/PluginNodeProxy.cpp
    src/plugin/PluginManager.cpp
    src/ui/panels/PluginManagerModal.cpp
    src/player/PlayerApp.cpp
)

target_include_directories(nodeforge_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui_backends
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pybind11/include
    ${Stb_INCLUDE_DIR}
    ${Python3_INCLUDE_DIRS}
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
    Python3::Python
    $<$<PLATFORM_ID:Windows>:ws2_32 winmm>
)

target_compile_options(nodeforge_core PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /wd4996 /wd4251 /wd4275 /wd4127 /wd4244 /wd4267>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wpedantic>
)

get_filename_component(PY_HOME "${Python3_INCLUDE_DIRS}" DIRECTORY)
message(STATUS "Python Home configured as: ${PY_HOME}")

target_compile_definitions(nodeforge_core PUBLIC
    NF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NF_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NF_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    NF_PYTHON_HOME=\"${PY_HOME}\"
    $<$<CONFIG:Debug>:NF_DEBUG=1>
)

# ─── Executables ────────────────────────────────────────────────────────────
add_executable(nodeforge
    src/app/main.cpp
)
target_link_libraries(nodeforge PRIVATE
    nodeforge_core
)

add_executable(nodeforge_player
    src/player/main_player.cpp
)
target_link_libraries(nodeforge_player PRIVATE
    nodeforge_core
)

# ─── Sample Plugins ─────────────────────────────────────────────────────────
add_subdirectory(sdk/samples/TexInvertPlugin)
add_subdirectory(sdk/samples/ChanHarmonicLFOPlugin)
add_subdirectory(sdk/samples/DataCSVTransformPlugin)

# ─── Tests & Benchmarks ─────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)

# ─── Installation & CPack Packaging ─────────────────────────────────────────
install(TARGETS nodeforge nodeforge_player
    RUNTIME DESTINATION bin
)

install(DIRECTORY sdk/include/
    DESTINATION sdk/include
)
install(DIRECTORY sdk/samples/
    DESTINATION sdk/samples
)
install(FILES sdk/README.md
    DESTINATION sdk
)

set(CPACK_PACKAGE_NAME "NodeForge")
set(CPACK_PACKAGE_VENDOR "Neo Realms")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NodeForge Real-Time Visual Operator & Projection Mapping Engine")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-v${PROJECT_VERSION}-win64")

set(CPACK_GENERATOR "ZIP;NSIS")
set(CPACK_NSIS_DISPLAY_NAME "NodeForge Studio")
set(CPACK_NSIS_PACKAGE_NAME "NodeForge Studio")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/package/nsis/nodeforge.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/package/nsis/nodeforge.ico")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

include(CPack)
