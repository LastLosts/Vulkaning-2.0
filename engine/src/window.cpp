#include "window.hpp"
#include <iostream>

namespace ving
{
Window::Window(const VulkanInstance &instance, uint32_t width, uint32_t height)
    : m_instance{instance.instance}, m_width{width}, m_height{height}
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, "Title", nullptr, nullptr);
    if (!m_window)
        std::cout << "Failed to create GLFW window\n";

    if (glfwCreateWindowSurface(instance.instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
        std::cout << "Failed to create window surface\n";
}
Window::~Window()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    glfwDestroyWindow(m_window);
}
} // namespace ving
