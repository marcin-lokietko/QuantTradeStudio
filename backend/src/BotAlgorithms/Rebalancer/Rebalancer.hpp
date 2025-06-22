#pragma once

#include <memory>
#include <thread>

#include "AssetSharesFloating.hpp"
#include "Config.hpp"
#include "MarketService/AssetValues.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/ITime.hpp"

namespace BotAlgorithms::Rebalancer {

class Rebalancer {
 public:
  Rebalancer(Config&& config, const MarketService::IMarketService& marketService, const Time::ITime& time);

  void run(std::stop_token st);

 private:
  void rebalance();
  void cancelOpenOrders();
  MarketService::AssetValues getRelevantOwnedAssetValues();
  std::optional<double> calcTotalValueOfRelevantOwnedAssets(const MarketService::AssetValues& relevantOwnedAssetValues);
  AssetSharesFloating getActualAssetShares(const MarketService::AssetValues& relevantOwnedAssetValues,
                                           const double totalValueOfRelevantOwnedAssets);

  const Config config_;
  const MarketService::IMarketService& marketService_;
  const Time::ITime& time_;
};

}  // namespace BotAlgorithms::Rebalancer