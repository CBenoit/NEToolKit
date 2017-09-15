cmake_minimum_required(VERSION 3.6)
project(NEToolKit)

set(CMAKE_CXX_STANDARD 17)

include_directories(NEToolKit/include)

# library
add_subdirectory(NEToolKit)

# examples
add_subdirectory(NEToolKitExamples)
