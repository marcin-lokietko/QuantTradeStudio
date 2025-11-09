#include "BotBacktester/Simulators/SystemTimeSimulator.hpp"

#include "Utils/ToString.hpp"
#include "spdlog/spdlog.h"

namespace BotBacktester::Simulators {

SystemTimeSimulator::SystemTimeSimulator(TimePoint simulationStart, TimePoint simulationEnd)
    : currentTime_(simulationStart), simulationEnd_(simulationEnd) {
  SPDLOG_INFO("SystemTimeSimulator initialized with start time: {}, end time: {}", ::toString(simulationStart),
              ::toString(simulationEnd));
}

void SystemTimeSimulator::sleepFor(std::stop_token st, const std::chrono::milliseconds& duration) const {
  (void)st;
  if (currentTime_ >= simulationEnd_) {
    SPDLOG_INFO("Simulation end reached at {}", ::toString(simulationEnd_));
    if (endCallback_) {
      endCallback_();
      endCallback_ = []() {};
    }
    return;
  }
  currentTime_ += duration;
  SPDLOG_INFO("Simulated sleepFor(duration={} ms), current simulation time: {}", duration.count(),
              ::toString(currentTime_));
}

time_t SystemTimeSimulator::getTimeSinceEpoch() const { return std::chrono::system_clock::to_time_t(currentTime_); }

void SystemTimeSimulator::registerSimulationEndCallback(std::function<void()> callback) {
  if (currentTime_ >= simulationEnd_) {
    SPDLOG_INFO("Simulation end reached, invoking callback immediately.");
    callback();
  } else {
    endCallback_ = std::move(callback);
  }
}
}  // namespace BotBacktester::Simulators