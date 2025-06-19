#include "Time.hpp"

#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>

namespace Time {

void Time::sleepFor(std::stop_token st, const ::std::chrono::milliseconds& duration) const {
  std::mutex mtx;
  std::condition_variable_any cv;
  std::unique_lock lock(mtx);

  std::stop_callback cb(st, [&cv]() { cv.notify_all(); });

  const auto start = std::chrono::steady_clock::now();
  while (!st.stop_requested()) {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    if (elapsed >= duration) {
      break;
    }
    cv.wait_for(lock, duration - elapsed, [&st] { return st.stop_requested(); });
  }
}

time_t Time::getTimeSinceEpoch() const {
  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) * 1000;
}

}  // namespace Time