#include <chrono>

#include "BotBacktester/Simulators/SystemTimeSimulator.hpp"
#include "gmock/gmock.h"

namespace BotBacktester::Simulators {

using testing::_;
using testing::Return;

class SystemTimeSimulatorTest : public ::testing::Test {
 public:
  const SystemTimeSimulator::TimePoint simulationStart_ = SystemTimeSimulator::TimePoint(
      std::chrono::sys_days(std::chrono::year{2025} / std::chrono::January / 1) + std::chrono::hours(12));

  const SystemTimeSimulator::TimePoint simulationEnd_ = simulationStart_ + std::chrono::hours(1);

  SystemTimeSimulator sut_{simulationStart_, simulationEnd_};
};

TEST_F(SystemTimeSimulatorTest, WhenSleepForCalled_ThenReturnedTimeIsAdvanced) {
  EXPECT_EQ(std::chrono::system_clock::to_time_t(simulationStart_), sut_.getTimeSinceEpoch());

  sut_.sleepFor(std::stop_token{}, std::chrono::milliseconds(100));
  EXPECT_EQ(std::chrono::system_clock::to_time_t(simulationStart_ + std::chrono::milliseconds(100)),
            sut_.getTimeSinceEpoch());
}

TEST_F(SystemTimeSimulatorTest, WhenSimulationEndTimeExceeded_ThenRegisteredCallbackIsInvoked) {
  bool callbackInvoked = false;
  sut_.registerSimulationEndCallback([&callbackInvoked]() { callbackInvoked = true; });

  sut_.sleepFor(std::stop_token{}, std::chrono::milliseconds(3600000));
  EXPECT_FALSE(callbackInvoked);
  sut_.sleepFor(std::stop_token{}, std::chrono::milliseconds(1));
  EXPECT_TRUE(callbackInvoked);
}

}  // namespace BotBacktester::Simulators
