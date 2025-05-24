#include "window.hpp"
#include <functional>
#include <iostream>

using namespace std::placeholders;

namespace ving
{
struct ResizeCallback
{
    static void callback(GLFWwindow *window, int width, int height)
    {
        Window *win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        win->m_width = width;
        win->m_height = height;
    }
};

Window::Window(uint32_t width, uint32_t height, bool resizable) : m_width{width}, m_height{height}
{
    if (!glfwInit())
        std::cout << "Failed to initialize glfw\n";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (resizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(width, height, "Title", nullptr, nullptr);
    if (!m_window)
        std::cout << "Failed to create GLFW window\n";

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, ResizeCallback::callback);
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
