#pragma once

#include <numeric>
#include <ranges>

#include "ApiGateway/BotConfig.hpp"
#include "Utils/StrongType.hpp"

namespace BotExecution::Rebalancer {

struct Config {
  ApiGateway::ExecutionPeriod executionPeriod{};
  ApiGateway::IsExecutedImmediately isExecutedImmediately{};
  ApiGateway::AssetSymbol quoteAsset{};
  ApiGateway::AssetShares baseAssetShares{};
};

inline bool isValid(const Config& config) {
  constexpr auto minBaseAssets = 2;
  if (config.baseAssetShares.size() < minBaseAssets) {
    return false;
  }

  const auto sharesView =
      config.baseAssetShares | std::views::transform([](const auto& item) { return item.sharePercent.val_; });
  const auto sumShares = std::reduce(sharesView.begin(), sharesView.end(), 0);

  return config.executionPeriod.val_ > 0 && sumShares == 100;
}

}  // namespace BotExecution::Rebalancer
