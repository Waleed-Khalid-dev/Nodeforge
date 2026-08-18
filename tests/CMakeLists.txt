add_executable(nodeforge_tests
    unit/gpu_device_test.cpp
    unit/graph_dag_test.cpp
    unit/operators_test.cpp
    benchmark/graph_benchmark.cpp
)

target_link_libraries(nodeforge_tests PRIVATE
    nodeforge_core
    GTest::gtest_main
    benchmark::benchmark
)

include(GoogleTest)
gtest_discover_tests(nodeforge_tests)
