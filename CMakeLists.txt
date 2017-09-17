cmake_minimum_required(VERSION 3.6)
project(NEToolKit)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Enable release mode by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

include(cmake/options.cmake)
include(cmake/config.cmake)

if(CMAKE_COMPILER_IS_GNUCXX)
    include(cmake/macros/gcc-settings.cmake)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    include(cmake/macros/clang-settings.cmake)
endif()

message(STATUS "${PROJECT_NAME} build type   : ${CMAKE_BUILD_TYPE}")

if(NETKIT_SHARED)
    message(STATUS "Build as shared library: Yes (default)")
else()
    message(STATUS "Build as shared library: No (as static)")
endif()

if(NETKIT_EXAMPLES)
    message(STATUS "Build examples         : Yes")
else()
    message(STATUS "Build examples         : No  (default)")
endif()

# library
set(NETOOLKIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/NEToolKit/include)
add_subdirectory(NEToolKit)

# examples
if(NETKIT_EXAMPLES)
    add_subdirectory(NEToolKitExamples)
endif()

