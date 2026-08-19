add_executable(nodeforge_tests
    unit/gpu_device_test.cpp
    unit/graph_dag_test.cpp
    unit/operators_test.cpp
    unit/parameter_test.cpp
    unit/python_embed_test.cpp
    unit/texop_test.cpp
    unit/texop_integration_test.cpp
    unit/ui_undo_test.cpp
    unit/ui_canvas_math_test.cpp
    unit/project_serialization_test.cpp
    unit/component_hierarchy_test.cpp
    benchmark/graph_benchmark.cpp
    benchmark/texop_benchmark.cpp
)

target_link_libraries(nodeforge_tests PRIVATE
    nodeforge_core
    GTest::gtest_main
    benchmark::benchmark
)

include(GoogleTest)
gtest_discover_tests(nodeforge_tests)
