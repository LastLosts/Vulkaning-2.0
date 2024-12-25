add_library(engine-dependencies INTERFACE)

find_package(Vulkan REQUIRED)

target_link_libraries(engine-dependencies INTERFACE Vulkan::Vulkan)
