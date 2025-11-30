#pragma once

#include <string>

#include "ApiGateway/KlineInterval.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "KlineSequence.hpp"

namespace MarketService {

class IHistoricalMarketDataProvider {
 public:
  virtual ~IHistoricalMarketDataProvider() = default;
  virtual KlineSequence getKlines(const ApiGateway::TradingPairSymbol& symbol,
                                  const ApiGateway::KlineInterval klineInterval,
                                  const std::chrono::system_clock::time_point startTime,
                                  const std::chrono::system_clock::time_point endTime) const = 0;
};

}  // namespace MarketService