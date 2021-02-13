include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(googletest)

add_executable(Tests
        src/VM.cpp
        src/Symbol.cpp
        src/classfile/ConstantPool.cpp
        src/BaseTest.cpp
        src/BaseTest.hpp
        main.cpp
        )
target_include_directories(Tests PRIVATE ../src)
target_link_libraries(Tests Tula gtest_main)
add_test(NAME example_test COMMAND Tests)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
