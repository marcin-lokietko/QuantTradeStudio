#pragma once

#include <chrono>

namespace Time {

class ITime {
 public:
  virtual ~ITime() = default;

  virtual void sleepFor(const ::std::chrono::seconds& seconds) const = 0;
};

}  // namespace Time