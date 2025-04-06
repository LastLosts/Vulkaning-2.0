file(GLOB_RECURSE lighting-sources "src/*.cpp")
file(GLOB_RECURSE lighting-headers "src/*.hpp")

add_subdirectory(shaders)

add_executable(lighting ${lighting-sources} ${lighting-headers})
target_link_libraries(lighting PRIVATE engine)

add_dependencies(lighting lighting-shaders)
