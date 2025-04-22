#pragma once

#include <thread>

#include "AssetSharesFloating.hpp"
#include "Config.hpp"
#include "MarketService/IMarketService.hpp"
#include "Wallet/AssetValues.hpp"
#include "Wallet/IWallet.hpp"

namespace BotExecution::Rebalancer {

class Rebalancer {
 public:
  Rebalancer(Config&& config, const MarketService::IMarketService& marketService, const Wallet::IWallet& wallet);

  void run(std::stop_token st);

 private:
  void rebalance();
  void cancelOpenOrders();
  Wallet::AssetValues getRelevantOwnedAssetValues();
  double calcTotalValueOfRelevantOwnedAssets(const Wallet::AssetValues& relevantOwnedAssetValues);
  AssetSharesFloating getActualAssetShares(const Wallet::AssetValues& relevantOwnedAssetValues,
                                           const double totalValueOfRelevantOwnedAssets);

  const Config config_;
  const MarketService::IMarketService& marketService_;
  const Wallet::IWallet& wallet_;
};

}  // namespace BotExecution::Rebalancer