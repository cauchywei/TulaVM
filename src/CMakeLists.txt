cmake_minimum_required(VERSION 3.14)
project(Tula
        VERSION 0.1
        LANGUAGES CXX
        )

include(FetchContent)
FetchContent_Declare(
        CCWPP
        GIT_REPOSITORY git@github.com:cauchywei/CCWPP.git
        GIT_TAG        82dea3c5f6f34dc090b12310e6f9bca8bb347b90
)
FetchContent_MakeAvailable(CCWPP)


set(TULA_SRC
        classfile/ClassFileParser.cpp
        classfile/ClassFileParser.hpp
        classfile/ClassFileReader.cpp
        classfile/ClassFileReader.hpp
        utils/Enum.hpp
        VM.cpp
        JVM.hpp
        Klass.cpp
        Klass.hpp
        ClazzLoader.cpp
        ClazzLoader.hpp
        Error.hpp
        Types.hpp
        ConstantPool.cpp
        ConstantPool.hpp
        Symbol.cpp
        Symbol.hpp
        SymbolTable.cpp
        SymbolTable.hpp)

add_library(Tula SHARED ${TULA_SRC})

target_include_directories(Tula PUBLIC ../include)
target_link_libraries(Tula CCWPP)