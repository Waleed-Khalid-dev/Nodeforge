add_executable(nodeforge_tests
    unit/gpu_device_test.cpp
    ../src/gpu/Device.cpp
)

target_link_libraries(nodeforge_tests PRIVATE
    GTest::gtest_main
    glfw
    Vulkan::Vulkan
    vk-bootstrap::vk-bootstrap
    GPUOpen::VulkanMemoryAllocator
    spdlog::spdlog
)

target_compile_options(nodeforge_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /wd4996>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wpedantic>
)

include(GoogleTest)
gtest_discover_tests(nodeforge_tests)
