#pragma once

#include <optional>

#include "ApiGateway/AssetQuantitiesAndValuesHistory.hpp"
#include "ApiGateway/AssetQuantity.hpp"
#include "ApiGateway/Percent.hpp"
#include "BotBacktester/BotAssetsHistory.hpp"
#include "Utils/StrongType.hpp"
#include "Utils/ToString.hpp"

namespace ApiGateway {

struct BacktestResults {
  std::optional<AssetQuantitiesAndValuesHistory> botAssetHistory{};
  std::optional<AssetQuantity> totalProfitOrLossInAbsolute{};
  std::optional<Percent> totalProfitOrLossInPercent{};

  std::optional<Percent> winRate{};
  std::optional<AssetQuantity> averageWinSizeInAbsolute{};
  std::optional<AssetQuantity> averageLossSizeInAbsolute{};

  std::optional<AssetQuantitiesAndValuesHistory> assetHistoryIfHeld{};
  std::optional<AssetQuantity> totalProfitOrLossInAbsoluteIfHeld{};
  std::optional<Percent> totalProfitOrLossInPercentIfHeld{};

  std::optional<AssetSymbol> absoluteAsset{};

  bool operator==(const BacktestResults& other) const = default;
};

// inline std::string toString(const BacktestResults& backtestResults) {
//   return std::format(
//       "{{botAssetHistory={}, totalProfitOrLossInAbsolute={}, totalProfitOrLossInPercent={}, winRate={}, "
//       "averageWinSizeInAbsolute={}, averageLossSizeInAbsolute={}, assetHistoryIfHeld={}, "
//       "totalProfitOrLossInAbsoluteIfHeld={}, totalProfitOrLossInPercentIfHeld={}, absoluteAsset={}}}",
//       backtestResults.botAssetHistory, backtestResults.totalProfitOrLossInAbsolute,
//       backtestResults.totalProfitOrLossInPercent, backtestResults.winRate, backtestResults.averageWinSizeInAbsolute,
//       backtestResults.averageLossSizeInAbsolute, backtestResults.assetHistoryIfHeld,
//       backtestResults.totalProfitOrLossInAbsoluteIfHeld, backtestResults.totalProfitOrLossInPercentIfHeld,
//       backtestResults.absoluteAsset);
// }
}  // namespace ApiGateway
