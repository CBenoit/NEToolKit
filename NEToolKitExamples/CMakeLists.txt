# Examples

include_directories(${NETOOLKIT_INCLUDE_DIR})
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable(NEToolKitExamples ${SOURCE_FILES})
target_link_libraries(NEToolKitExamples $<TARGET_FILE:NEToolKit>)
