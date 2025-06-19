#pragma once

#include <chrono>
#include <stop_token>

namespace Time {

class ITime {
 public:
  virtual ~ITime() = default;

  virtual void sleepFor(std::stop_token st, const ::std::chrono::milliseconds& duration) const = 0;
  virtual time_t getTimeSinceEpoch() const = 0;
};

}  // namespace Time