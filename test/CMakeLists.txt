cmake_minimum_required (VERSION 2.6)

set(PROJECT_NAME MINICMD_TEST)
project(${PROJECT_NAME})

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -O3" )

INCLUDE_DIRECTORIES("..")

# applications
FILE(GLOB incs "*.hpp")
FILE(GLOB srcs "*.cpp")

ADD_EXECUTABLE(minicmd_test ${incs} ${srcs})
