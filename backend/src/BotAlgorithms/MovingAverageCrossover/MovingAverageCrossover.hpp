#pragma once

#include <memory>
#include <thread>

#include "BotAlgorithms/MovingAverageCrossover/AssetHistory.hpp"
#include "BotAlgorithms/MovingAverageCrossover/AverageCalculator.hpp"
#include "Config.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/ITime.hpp"

namespace BotAlgorithms::MovingAverageCrossover {

class MovingAverageCrossover {
 public:
  MovingAverageCrossover(Config&& config, const MarketService::IMarketService& marketService, const Time::ITime& time);

  void run(std::stop_token st);

 private:
  void performIteration();
  void addHistoryForAllAssets();
  bool isMinimalHistoryCollectedForAllAssets() const;
  void placeOrdersBasedOnCrossoverSignals();

  std::map<ApiGateway::AssetSymbol, AssetHistory> assetHistories_;

  const Config config_;
  const MarketService::IMarketService& marketService_;
  const Time::ITime& time_;
};

}  // namespace BotAlgorithms::MovingAverageCrossover
