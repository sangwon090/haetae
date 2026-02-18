cmake_minimum_required(VERSION 3.22)
project (Haetae LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)

set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/expr.cpp
    src/parser/parser.cpp
)

add_library(haetae_lib ${SOURCES})

target_include_directories(haetae_lib PRIVATE ${LLLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS})
target_compile_definitions(haetae_lib PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(haetae_lib PRIVATE
    MLIRIR
    MLIRSupport
    MLIRParser
)



add_executable(haetae src/main.cpp)
target_link_libraries(haetae PRIVATE haetae_lib)

option(ENABLE_FUZZING "Build fuzz targets" OFF)

if(ENABLE_FUZZING)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Fuzzing requires Clang")
    endif()

    add_executable(fuzz_parser fuzz/fuzz_parser.cpp)
    target_link_libraries(fuzz_parser PRIVATE haetae_lib)
    target_compile_options(fuzz_parser PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer
        -g
    )
    target_link_options(fuzz_parser PRIVATE
        -fsanitize=fuzzer,address,undefined
    )
endif()