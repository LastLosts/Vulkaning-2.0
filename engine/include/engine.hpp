#pragma once

#include "vulkan_core.hpp"

namespace ving {
class Engine {

public:
  Engine();

  [[nodiscard]] bool running() { return m_running; }
  void run();
  void update();

private:
  bool m_running;

  VulkanCore m_core;
};

} // namespace ving
