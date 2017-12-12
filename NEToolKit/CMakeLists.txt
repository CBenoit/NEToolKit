# Library

include_directories(${NETOOLKIT_INCLUDE_DIR})
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "include/*.h" "include/*.tpp")
add_library(NEToolKit ${SOURCE_FILES})
