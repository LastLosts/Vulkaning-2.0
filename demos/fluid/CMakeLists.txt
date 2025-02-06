file(GLOB_RECURSE fluid-sources "src/*.cpp")
file(GLOB_RECURSE fluid-headers "src/*.hpp")

# add_subdirectory(shaders)

add_executable(fluid ${fluid-sources} ${fluid-headers})
target_link_libraries(fluid PRIVATE engine)

# add_dependencies(fluid slime-shaders)
