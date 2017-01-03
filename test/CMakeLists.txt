cmake_minimum_required (VERSION 2.6)

set(PROJECT_NAME MINICMD_TEST)
project(${PROJECT_NAME})

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -O3" )

INCLUDE_DIRECTORIES("..")
INCLUDE_DIRECTORIES("googletest-release-1.8.0/googletest/include")

# add gtest (building with same compiler is best option)
add_subdirectory(googletest-release-1.8.0)
config_compiler_and_linker()

# applications
ADD_EXECUTABLE(test_app test.cpp)
ADD_EXECUTABLE(unit_test unit_test.cpp)
TARGET_LINK_LIBRARIES(unit_test gtest gtest_main)
