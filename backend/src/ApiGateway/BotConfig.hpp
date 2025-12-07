#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <vector>

#include "ApiGateway/KlineInterval.hpp"
#include "AssetHistoryLength.hpp"
#include "AssetSymbols.hpp"
#include "KlineInterval.hpp"
#include "Utils/StrongType.hpp"
#include "Utils/ToString.hpp"

namespace ApiGateway {

DEFINE_STRONG_TYPE(BotName, std::string);
DEFINE_STRONG_TYPE(ExecutionPeriod, uint64_t);
DEFINE_STRONG_TYPE(SharePercent, uint64_t);
DEFINE_STRONG_TYPE(IsExecutedImmediately, bool);

struct SingleAssetShare {
  AssetSymbol assetSymbol{};
  SharePercent sharePercent{};

  bool operator==(const SingleAssetShare& other) const = default;
};

inline std::string toString(const SingleAssetShare& singleAssetShare) {
  return std::format("{{assetSymbol={}, sharePercent={}}}", singleAssetShare.assetSymbol.val_,
                     singleAssetShare.sharePercent.val_);
}

using AssetShares = std::vector<SingleAssetShare>;

struct BotConfig {
  // Common
  BotName botName{};
  std::optional<AssetSymbol> quoteAsset{};
  std::optional<ExecutionPeriod> executionPeriod{};

  // Rebalancer specific
  std::optional<IsExecutedImmediately> isExecutedImmediately{};
  std::optional<AssetShares> baseAssetShares{};

  // Moving Average Crossover specific
  std::optional<AssetHistoryLength> shortTermMovingAverageLength{};
  std::optional<AssetHistoryLength> longTermMovingAverageLength{};
  std::optional<AssetSymbols> baseAssets{};

  // Donchian Channel Breakout Strategy specific
  std::optional<KlineInterval> executionInterval{KlineInterval::OneMinute};
  std::optional<AssetHistoryLength> entryChannelLength{};
  std::optional<AssetHistoryLength> exitChannelLength{};

  bool operator==(const BotConfig& other) const = default;
};

inline std::string toString(const BotConfig& botConfig) {
  return std::format(
      "{{botName={}, quoteAsset={}, executionPeriod={}, isExecutedImmediately={}, baseAssetShares={}, "
      "shortTermMovingAverageLength={}, longTermMovingAverageLength={}, baseAssets={}, executionInterval={}, "
      "entryChannelLength={}, exitChannelLength={}}}",
      botConfig.botName.val_, ::toString(botConfig.quoteAsset), ::toString(botConfig.executionPeriod),
      ::toString(botConfig.isExecutedImmediately), ::toString(botConfig.baseAssetShares),
      ::toString(botConfig.shortTermMovingAverageLength), ::toString(botConfig.longTermMovingAverageLength),
      ::toString(botConfig.baseAssets),
      botConfig.executionInterval ? toReadableString(*botConfig.executionInterval) : "nullopt",
      ::toString(botConfig.entryChannelLength), ::toString(botConfig.exitChannelLength));
}
}  // namespace ApiGateway
