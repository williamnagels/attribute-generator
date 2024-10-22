cmake_minimum_required(VERSION 3.10)
project(PythonClassGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(Clang REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(PythonClassGenerator PythonClassGenerator.cpp)

llvm_map_components_to_libnames(llvm_libs support)

target_link_libraries(PythonClassGenerator
  PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
    libclang
    ${llvm_libs}
)
