#include <random>

#include <nexus/pool.h>

int main() {
  nexus::pool pool(8);

  std::vector<std::future<std::string>> results;

  for (int i = 0; i < 500; ++i) {
    results.emplace_back(pool.enqueue([i] {
      for (int j = 0; j < 100000000; j++) {
        continue;
      }
      logger::success("Task " + std::to_string(i) + " finished");
      return std::to_string(i) + " -> " + std::to_string(rand() % 2 == 0);
    }));
  }

  for (std::future<std::string> &result : results) {
    logger::info("Result: " + result.get());
  }

  return 0;
}
