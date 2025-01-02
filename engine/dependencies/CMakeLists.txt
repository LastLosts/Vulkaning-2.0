add_library(engine-dependencies INTERFACE)

find_package(Vulkan REQUIRED)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(VulkanMemoryAllocator)

target_link_libraries(engine-dependencies INTERFACE Vulkan::Vulkan SDL3::SDL3 GPUOpen::VulkanMemoryAllocator)
target_compile_options(engine-dependencies INTERFACE -w)
