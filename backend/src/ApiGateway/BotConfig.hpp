#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <vector>

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
  std::optional<ApiGateway::KlineInterval> executionInterval{ApiGateway::KlineInterval::OneMinute};
  std::optional<ApiGateway::AssetHistoryLength> entryChannelLength{};
  std::optional<ApiGateway::AssetHistoryLength> exitChannelLength{};

  bool operator==(const BotConfig& other) const = default;
};

inline std::string toString(const BotConfig& botConfig) {
  return std::format("{{botName={}, executionPeriod={}, isExecutedImmediately={}, quoteAsset={}, baseAssetShares={}}}",
                     botConfig.botName.val_, ::toString(botConfig.executionPeriod),
                     ::toString(botConfig.isExecutedImmediately), ::toString(botConfig.quoteAsset),
                     ::toString(botConfig.baseAssetShares));
}
}  // namespace ApiGateway
