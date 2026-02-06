cmake_minimum_required(VERSION 3.24)
project (Haetae LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)

add_executable(haetae src/haetae.cpp)

target_include_directories(haetae PRIVATE ${LLLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS})
target_compile_definitions(haetae PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(haetae PRIVATE
    MLIRIR
    MLIRSupport
    MLIRParser
)