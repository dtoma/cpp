add_executable(stringTests test.cpp)
target_link_libraries(stringTests PRIVATE gtest_main)
gtest_discover_tests(stringTests)