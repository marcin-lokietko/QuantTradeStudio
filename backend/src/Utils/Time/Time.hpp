#pragma once

#include "ITime.hpp"

namespace Time {

class Time : public ITime {
 public:
  void sleepFor(std::stop_token st, const ::std::chrono::milliseconds& duration) const override;
};

}  // namespace Time