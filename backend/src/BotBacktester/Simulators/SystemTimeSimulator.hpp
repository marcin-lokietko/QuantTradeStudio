#pragma once

#include <functional>

#include "Utils/Time/ITime.hpp"

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

  SystemTimeSimulator(TimePoint simulationStart, TimePoint simulationEnd);

  void sleepFor(std::stop_token st, const std::chrono::milliseconds& duration) const override;

  time_t getTimeSinceEpoch() const override;

  void registerSimulationEndCallback(std::function<void()> callback);

 private:
  mutable TimePoint currentTime_;
  const TimePoint simulationEnd_;
  mutable std::function<void()> endCallback_;
};

}  // namespace BotBacktester::Simulators