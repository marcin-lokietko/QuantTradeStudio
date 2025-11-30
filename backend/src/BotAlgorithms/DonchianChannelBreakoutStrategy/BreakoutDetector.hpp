#pragma once

#include <functional>
#include <thread>

#include "Config.hpp"
#include "ExtremumTracker.hpp"
#include "MarketService/IHistoricalMarketDataProvider.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/ITime.hpp"

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

class BreakoutDetector {
 public:
  BreakoutDetector(ApiGateway::AssetHistoryLength entryChannelLength, ApiGateway::AssetHistoryLength exitChannelLength)
      : entryChannelMaxValue_(entryChannelLength.val_), exitChannelMinValue_(exitChannelLength.val_) {}

  void processKlines(const MarketService::KlineSequence& klines) {
    entryChannelMaxValue_.clear();
    exitChannelMinValue_.clear();

    currentclosePrice_ = std::stod(klines.back().closePrice.val_);

    // Exclude the last kline
    for (const auto& kline : klines | std::views::take(klines.size() - 1)) {
      entryChannelMaxValue_.addValue(std::stod(kline.highPrice.val_));
      exitChannelMinValue_.addValue(std::stod(kline.lowPrice.val_));
    }
  }

  bool isSellSignalled() const {
    if (!currentclosePrice_.has_value()) {
      return false;
    }

    if (const auto isCurrentCloseLowest = exitChannelMinValue_.isValueMoreExtreme(currentclosePrice_.value());
        isCurrentCloseLowest.has_value()) {
      return isCurrentCloseLowest.value();
    }
    return false;
  }

  bool isBuySignalled() const {
    if (!currentclosePrice_.has_value()) {
      return false;
    }

    if (const auto isCurrentCloseHighest = entryChannelMaxValue_.isValueMoreExtreme(currentclosePrice_.value());
        isCurrentCloseHighest.has_value()) {
      return isCurrentCloseHighest.value();
    }
    return false;
  }

 private:
  ExtremumTracker<double, std::less<double>> entryChannelMaxValue_;
  ExtremumTracker<double, std::greater<double>> exitChannelMinValue_;
  std::optional<double> currentclosePrice_;
};

}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy
