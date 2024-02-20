#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <logger/log.h>

namespace nexus {

class pool {
private:
  bool isRunning;
  std::mutex mutex;
  std::condition_variable condition;
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

public:
  pool(size_t numThreads);
  ~pool();

  template <class F, class... Args>
  std::future<typename std::result_of<F(Args...)>::type>
  enqueue(F &&f, Args &&...args) {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> result = task->get_future();
    {
      std::unique_lock<std::mutex> lock(mutex);
      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
  }
};

} // namespace nexus