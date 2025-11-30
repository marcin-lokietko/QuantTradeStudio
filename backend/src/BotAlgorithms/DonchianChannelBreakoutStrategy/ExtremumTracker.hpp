#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <optional>
#include <ranges>

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

template <typename T, typename IsRhsMoreExtreme>
class ExtremumTracker {
 public:
  explicit ExtremumTracker(uint32_t historySize) : historySize_(historySize) {}

  void addValue(T value) {
    if (history_.size() + 1 > historySize_) {
      if (currentExtreme_ == history_.front()) {
        currentExtreme_ = std::nullopt;
      }
      history_.pop_front();
    }

    history_.push_back(value);
    if (!currentExtreme_.has_value() || isRhsMoreExtreme_(currentExtreme_.value(), value)) {
      currentExtreme_ = value;
    }
  }

  std::optional<bool> isValueMoreExtreme(const auto& value) const {
    if (history_.size() != historySize_) {
      return std::nullopt;
    }
    if (!currentExtreme_.has_value()) {
      currentExtreme_ = *std::ranges::max_element(history_, isRhsMoreExtreme_);
    }
    return isRhsMoreExtreme_(currentExtreme_.value(), value);
  }

  void clear() {
    history_.clear();
    currentExtreme_ = std::nullopt;
  }

 private:
  std::deque<T> history_{};
  mutable std::optional<T> currentExtreme_{};
  const size_t historySize_{};
  IsRhsMoreExtreme isRhsMoreExtreme_{};
};

}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy
