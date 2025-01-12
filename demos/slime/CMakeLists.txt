file(GLOB_RECURSE slime-sources "src/*.cpp")
file(GLOB_RECURSE slime-headers "src/*.hpp")

add_subdirectory(shaders)

add_executable(slime ${slime-sources} ${slime-headers})
target_link_libraries(slime PRIVATE engine)

add_dependencies(slime slime-shaders)
