add_executable(Deap_test Deap.cpp test.cpp)
target_link_libraries(Deap_test GTest::gtest_main)

add_test(
    NAME "Deap Unit Test"
    COMMAND Deap_test
)