cmake_minimum_required(VERSION 3.15)
project(Chapter02 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
find_package(LLVM REQUIRED HINTS ${CMAKE_MODULE_PATH})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(AddLLVM)
include(HandleLLVMOptions)

include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIRS}")

llvm_map_components_to_libnames(llvm_libs support core irreader)


add_subdirectory(chapter02)