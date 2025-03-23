#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
  public:
    ThreadPool(size_t size) : m_stop{false}
    {
        for (size_t i = 0; i < size; ++i)
        {
            m_threads.emplace_back([this, i] {
                for (;;)
                {
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
                    if (m_stop && m_tasks.empty())
                        return;
                    auto task = std::move(m_tasks.front());
                    m_tasks.pop();
                    lock.unlock();
                    /*std::cout << "Starting task\n";*/
                    /*std::cout << m_done[i] << '\n';*/
                    m_threads_in_work.fetch_add(1);
                    task();
                    m_threads_in_work.fetch_sub(1);
                    /*std::cout << "Ending task\n";*/
                    /*std::cout << m_done[i] << '\n';*/
                }
            });
        }
    }

    template <class F> void enqueue(F &&task)
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_tasks.emplace(std::forward<F>(task));
        lock.unlock();
        m_condition.notify_one();
    }

    void wait()
    {
        while (done() == false)
        {
        }
    }

    [[nodiscard]] bool done() { return m_threads_in_work.load() == 0; }

    ~ThreadPool()
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_stop = true;
        lock.unlock();
        m_condition.notify_all();

        for (std::thread &t : m_threads)
        {
            t.join();
        }
    }

  private:
    std::atomic<int> m_threads_in_work;
    bool m_stop;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;

    std::condition_variable m_condition;
    std::mutex m_queue_mutex;
};
