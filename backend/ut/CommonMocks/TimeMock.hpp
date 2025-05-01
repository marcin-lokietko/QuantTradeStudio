#pragma once

#include <gmock/gmock.h>

#include "Utils/Time/ITime.hpp"

namespace Time {

class TimeMock : public ITime {
 public:
  MOCK_METHOD(void, sleepFor, (const ::std::chrono::seconds& seconds), (const, override));
};
}  // namespace Time