#include <chrono>
#include <string>

struct TimeResult
{
    std::string name;
    float time;
};

class Timer
{
  public:
    Timer() { m_start = std::chrono::high_resolution_clock::now(); }

    TimeResult get_result(std::string name)
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - m_start;
        return TimeResult{name, duration.count() * 1000.0f};
    }

  private:
    std::chrono::high_resolution_clock::time_point m_start;
};
