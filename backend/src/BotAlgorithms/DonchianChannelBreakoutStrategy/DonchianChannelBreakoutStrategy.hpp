#pragma once

#include <thread>
#include <vector>

#include "BreakoutDetector.hpp"
#include "Config.hpp"
#include "MarketService/IHistoricalMarketDataProvider.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/ITime.hpp"

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

class DonchianChannelBreakoutStrategy {
 public:
  DonchianChannelBreakoutStrategy(Config&& config, const MarketService::IMarketService& marketService,
                                  const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider,
                                  const Time::ITime& time);

  void run(std::stop_token st);

 private:
  void performIteration();
  void processKlinesForAllDetectors();
  void placeOrdersBasedOnDetectorSignals();

  const Config config_;
  const MarketService::IMarketService& marketService_;
  const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider_;
  const Time::ITime& time_;
  std::map<ApiGateway::AssetSymbol, BreakoutDetector> detectors_;
};

}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy
