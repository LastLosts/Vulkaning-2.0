#include "window.hpp"
#include <iostream>

namespace ving
{
Window::Window(uint32_t width, uint32_t height) : m_width{width}, m_height{height}
{
    if (!glfwInit())
        std::cout << "Failed to initialize glfw\n";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, "Title", nullptr, nullptr);
    if (!m_window)
        std::cout << "Failed to create GLFW window\n";
}
Window::~Window()
{
    glfwDestroyWindow(m_window);
}
[[nodiscard]] int Window::get_key(int key)
{
    return glfwGetKey(m_window, key);
}
[[nodiscard]] VkSurfaceKHR Window::create_vulkan_surface(VkInstance instance) const
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS)
        std::cout << "Failed to create window surface\n";
    return surface;
}
} // namespace ving
