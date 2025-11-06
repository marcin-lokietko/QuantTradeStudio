#pragma once

#include "Utils/Time/ITime.hpp"
#include "Utils/ToString.hpp"
#include "spdlog/spdlog.h"

namespace BotBacktester::Simulators {

class SystemTimeSimulatorException : public std::exception {
 public:
  explicit SystemTimeSimulatorException(std::string message) : message_(std::move(message)) {}

  const char* what() const noexcept override {
    return ("SystemTimeSimulatorException; Method not implemented: " + message_).c_str();
  }

 private:
  std::string message_;
};

class SystemTimeSimulator : public Time::ITime {
 public:
  using TimePoint = std::chrono::system_clock::time_point;

  SystemTimeSimulator(TimePoint simulationStart, TimePoint simulationEnd)
      : currentTime_(simulationStart), simulationEnd_(simulationEnd) {
    SPDLOG_INFO("SystemTimeSimulator initialized with start time: {}, end time: {}", ::toString(simulationStart),
                ::toString(simulationEnd));
  }

  void sleepFor(std::stop_token st, const std::chrono::milliseconds& duration) const override {
    (void)st;
    currentTime_ += duration;

    if (currentTime_ > simulationEnd_) {
      SPDLOG_INFO("Simulation end reached at {}", ::toString(simulationEnd_));
      if (endCallback_) {
        endCallback_();
        endCallback_ = []() {};
      }
      return;
    }
    SPDLOG_INFO("Simulated sleepFor(duration={} ms), current simulation time: {}", duration.count(),
                ::toString(currentTime_));
  }

  time_t getTimeSinceEpoch() const override { return std::chrono::system_clock::to_time_t(currentTime_); }

  void registerSimulationEndCallback(std::function<void()> callback) {
    if (currentTime_ >= simulationEnd_) {
      SPDLOG_INFO("Simulation end reached, invoking callback immediately.");
      callback();
    } else {
      endCallback_ = std::move(callback);
    }
  }

 private:
  mutable TimePoint currentTime_;
  const TimePoint simulationEnd_;
  mutable std::function<void()> endCallback_;
};

}  // namespace BotBacktester::Simulators