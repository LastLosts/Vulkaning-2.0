file(GLOB_RECURSE engine-sources "src/*.cpp")
file(GLOB_RECURSE engine-headers "include/*.hpp")

file(GLOB_RECURSE utility-sources "utility/*.cpp")
file(GLOB_RECURSE utility-headers "utility/*.hpp")

add_subdirectory(dependencies)

add_library(engine STATIC ${engine-sources} ${engine-headers} ${utility-sources} ${utility-headers})
target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(engine PUBLIC engine-dependencies)

target_compile_features(engine PUBLIC cxx_std_20)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(engine PRIVATE .)

