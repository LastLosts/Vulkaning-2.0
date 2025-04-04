add_library(engine-dependencies INTERFACE)

find_package(Vulkan REQUIRED)

set(BUILD_SHARED_LIBS off)

# set(SDL_SHARED OFF)
# set(SDL_STATIC ON)
# add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_subdirectory(VulkanMemoryAllocator)

set(GLM_BUILD_TESTS off)
add_subdirectory(glm)

set(GLFW_BUILD_DOCS off)
add_subdirectory(glfw)

add_library(imgui STATIC)

target_include_directories(imgui PUBLIC imgui)

target_sources(
  imgui
  PRIVATE imgui/imgui.h
          imgui/imgui.cpp
          imgui/imgui_demo.cpp
          imgui/imgui_draw.cpp
          imgui/imgui_widgets.cpp
          imgui/imgui_tables.cpp
          imgui/backends/imgui_impl_vulkan.cpp
          # imgui/backends/imgui_impl_sdl3.cpp)
          imgui/backends/imgui_impl_glfw.cpp)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

target_link_libraries(engine-dependencies INTERFACE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator imgui glm::glm)
