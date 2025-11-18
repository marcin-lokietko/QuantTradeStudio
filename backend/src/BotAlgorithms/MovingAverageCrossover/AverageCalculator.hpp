#pragma once

#include <cstdint>
#include <deque>
#include <optional>

namespace BotAlgorithms::MovingAverageCrossover {

template <typename T>
class AverageCalculator {
 public:
  explicit AverageCalculator(uint32_t historySize) : historySize_(historySize) {}

  void addValue(T value) {
    history_.push_back(value);
    sum_ += value;
    if (history_.size() > historySize_) {
      sum_ -= history_.front();
      history_.pop_front();
    }
  }

  std::optional<T> getAverage() const {
    if (history_.size() != historySize_) {
      return std::nullopt;
    }
    return sum_ / static_cast<T>(history_.size());
  }

 private:
  std::deque<T> history_;
  T sum_{};
  size_t historySize_{};
};

}  // namespace BotAlgorithms::MovingAverageCrossover
