#pragma once

#include "ITime.hpp"

namespace Time {

class Time : public ITime {
 public:
  void sleepFor(const ::std::chrono::seconds& seconds) const override;
};

}  // namespace Time