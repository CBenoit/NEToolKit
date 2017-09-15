# Examples

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable(NEToolKitExamples ${SOURCE_FILES})
target_link_libraries(NEToolKitExamples ${PROJECT_BINARY_DIR}/NEToolKit/libNEToolKit.a)
