#pragma once

#include "ITime.hpp"

namespace Time {

class SystemTime : public ITime {
 public:
  void sleepFor(std::stop_token st, const ::std::chrono::milliseconds& duration) const override;

  time_t getTimeSinceEpoch() const override;
};

}  // namespace Time