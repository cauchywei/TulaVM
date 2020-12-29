cmake_minimum_required(VERSION 3.14)
project(TulaVM)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(CCWPP)

add_subdirectory(src)

add_subdirectory(tests)