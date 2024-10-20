add_executable(MinMaxHeap_test MinMaxHeap.cpp test.cpp)
target_link_libraries(MinMaxHeap_test GTest::gtest_main)

add_test(
    NAME "MinMaxHeap Unit Test"
    COMMAND MinMaxHeap_test    
)