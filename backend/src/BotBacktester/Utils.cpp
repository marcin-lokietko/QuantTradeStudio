#include "BotBacktester/Utils.hpp"

#include <spdlog/spdlog.h>

namespace BotBacktester {

bool doKlinesHaveDifferentSizesOrTimeRanges(
    const std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence>& klines) {
  if (klines.empty()) {
    return false;
  }

  const auto firstSize = klines.begin()->second.size();
  for (const auto& [symbol, klineSequence] : klines) {
    if (klineSequence.size() != firstSize) {
      SPDLOG_ERROR("Kline length mismatch. Length for symbol={} is {}; and for symbol={} is {}.",
                   toString(klines.begin()->first), firstSize, toString(symbol), klineSequence.size());
      return true;
    }
  }

  for (size_t i = 0; i < firstSize; ++i) {
    const auto& firstKline = klines.begin()->second[i];
    for (const auto& [symbol, klineSequence] : klines) {
      const auto& currentKline = klineSequence[i];
      if (currentKline.openTime != firstKline.openTime || currentKline.closeTime != firstKline.closeTime) {
        SPDLOG_ERROR(
            "Kline time range mismatch at index={}. Time range for symbol={} is [{}, {}]; and for symbol={} is "
            "[{}, {}]",
            i, toString(klines.begin()->first), firstKline.openTime.time_since_epoch().count(),
            firstKline.closeTime.time_since_epoch().count(), toString(symbol),
            currentKline.openTime.time_since_epoch().count(), currentKline.closeTime.time_since_epoch().count());
        return true;
      }
    }
  }

  return false;
}

}  // namespace BotBacktester