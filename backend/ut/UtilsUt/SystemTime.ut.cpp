#include "Utils/Time/SystemTime.hpp"

#include "gmock/gmock.h"

namespace Time {

TEST(TimeTest, sleepForSleepsForSpecifiedDuration) {
  const std::chrono::milliseconds numMilliseconds{100};

  const auto start = std::chrono::steady_clock::now();
  SystemTime().sleepFor(std::stop_token{}, numMilliseconds);
  const auto end = std::chrono::steady_clock::now();

  auto actualDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  const auto tolerance = std::chrono::milliseconds{10};
  EXPECT_GE(actualDuration, numMilliseconds - tolerance);
  EXPECT_LT(actualDuration, numMilliseconds + tolerance);
}
}  // namespace Time
