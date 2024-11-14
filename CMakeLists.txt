cmake_minimum_required(VERSION 3.10)
project(transfer-function-analysis)

# Find LLVM packages
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM includes and definitions
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Set C++ standard to match LLVM's requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create executable
add_executable(transfer-function-analysis transfer-function-analysis.cpp)

# Find required LLVM components
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(transfer-function-analysis ${llvm_libs})

# Set warning flags
target_compile_options(transfer-function-analysis PRIVATE -Wall -Wextra)

# Print LLVM compilation flags
message(STATUS "LLVM compilation flags: ${LLVM_CXXFLAGS}")
