#include "engine.hpp"
#include <iostream>

int main() {
  ving::Engine engine{};

  engine.run();

  while (engine.running()) {
    engine.update();
  }
  return 0;
}
