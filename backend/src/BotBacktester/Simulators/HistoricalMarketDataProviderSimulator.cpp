#include "BotBacktester/Simulators/HistoricalMarketDataProviderSimulator.hpp"

#include "BotBacktester/Utils.hpp"
#include "Utils/ToString.hpp"

namespace BotBacktester::Simulators {

HistoricalMarketDataProviderSimulator::HistoricalMarketDataProviderSimulator(
    std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines)
    : klines_(std::move(klines)) {
  if (doKlinesHaveDifferentSizesOrTimeRanges(klines_)) {
    throw HistoricalMarketDataProviderSimulatorException(
        "Contract broken - klines provided to MarketServiceSimulator have different sizes or time ranges");
  }
}

MarketService::KlineSequence HistoricalMarketDataProviderSimulator::getKlines(
    const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::KlineInterval klineInterval,
    const std::chrono::system_clock::time_point startTime, const std::chrono::system_clock::time_point endTime) const {
  const auto& sourceKlines = getKlineSequence(symbol);

  throwIfSourceKlinesNotSuitableRequestedTimeRange(sourceKlines, klineInterval, startTime, endTime);

  MarketService::KlineSequence result;
  auto currentWindowStart = startTime;
  while (currentWindowStart < endTime) {
    auto currentWindowEnd = currentWindowStart + toMilliseconds(klineInterval);

    // TODO optimize - does not make sense to search from begin every time
    std::vector<const MarketService::Kline*> windowKlines;
    for (const auto& kline : sourceKlines) {
      const bool isKlineCompletelyBeforeWindow = kline.closeTime <= currentWindowStart;
      if (isKlineCompletelyBeforeWindow) {
        continue;
      }
      const bool isKlineCompletelyAfterWindow = kline.openTime >= currentWindowEnd;
      if (isKlineCompletelyAfterWindow) {
        break;
      }
      windowKlines.push_back(&kline);
    }

    if (!windowKlines.empty()) {
      MarketService::Kline aggregatedResultKline;
      aggregatedResultKline.openTime = currentWindowStart;
      aggregatedResultKline.closeTime = currentWindowEnd;
      aggregatedResultKline.openPrice = windowKlines.front()->openPrice;
      aggregatedResultKline.closePrice = windowKlines.back()->closePrice;

      double maxPrice = std::numeric_limits<double>::lowest();
      double minPrice = std::numeric_limits<double>::max();
      for (const auto* kline : windowKlines) {
        maxPrice = std::max(maxPrice, std::stod(kline->highPrice.val_));
        minPrice = std::min(minPrice, std::stod(kline->lowPrice.val_));
      }

      aggregatedResultKline.highPrice = ApiGateway::Price{std::to_string(maxPrice)};
      aggregatedResultKline.lowPrice = ApiGateway::Price{std::to_string(minPrice)};

      result.push_back(aggregatedResultKline);
    }

    currentWindowStart = currentWindowEnd;
  }

  SPDLOG_INFO("Simulating getKlines(symbol={}, klineInterval={}, startTime={}, endTime={}); returning {} klines",
              toString(symbol), toReadableString(klineInterval), ::toString(startTime), ::toString(endTime),
              result.size());

  return result;
}

const MarketService::KlineSequence& HistoricalMarketDataProviderSimulator::getKlineSequence(
    const ApiGateway::TradingPairSymbol& symbol) const {
  auto it = klines_.find(symbol);
  if (it == klines_.end()) {
    SPDLOG_ERROR("No kline data available for symbol={}", toString(symbol));
    throw HistoricalMarketDataProviderSimulatorException("No kline data available for symbol=" + toString(symbol));
  }
  if (it->second.empty()) {
    SPDLOG_ERROR("Kline sequence is empty for symbol={}", toString(symbol));
    throw HistoricalMarketDataProviderSimulatorException("Kline sequence is empty for symbol=" + toString(symbol));
  }
  return it->second;
}

void HistoricalMarketDataProviderSimulator::throwIfSourceKlinesNotSuitableRequestedTimeRange(
    const MarketService::KlineSequence& sourceKlines, const ApiGateway::KlineInterval requestedKlineInterval,
    const std::chrono::system_clock::time_point startTime, const std::chrono::system_clock::time_point endTime) const {
  const auto& firstKline = sourceKlines.front();
  const auto& lastKline = sourceKlines.back();

  if (startTime < firstKline.openTime) {
    SPDLOG_ERROR("Requested startTime={} is before first available kline openTime={}", ::toString(startTime),
                 ::toString(firstKline.openTime));
    throw HistoricalMarketDataProviderSimulatorException("Requested startTime is before first available kline");
  }

  if (endTime > lastKline.closeTime) {
    SPDLOG_ERROR("Requested endTime={} is after last available kline closeTime={}", ::toString(endTime),
                 ::toString(lastKline.closeTime));
    throw HistoricalMarketDataProviderSimulatorException("Requested endTime is after last available kline");
  }

  const auto sourceIntervalMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(firstKline.closeTime - firstKline.openTime);
  const auto targetIntervalMs = toMilliseconds(requestedKlineInterval);

  if (targetIntervalMs < sourceIntervalMs) {
    SPDLOG_ERROR("Requested klineInterval={} is smaller than source interval={}ms. This is not supported.",
                 toReadableString(requestedKlineInterval), sourceIntervalMs.count());
    throw HistoricalMarketDataProviderSimulatorException(
        "Requested klineInterval is smaller than source data granularity. This is not supported.");
  }
}

}  // namespace BotBacktester::Simulators