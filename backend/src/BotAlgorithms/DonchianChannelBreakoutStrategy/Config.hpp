#pragma once

#include "ApiGateway/AssetHistoryLength.hpp"
#include "ApiGateway/BotConfig.hpp"
#include "ApiGateway/KlineInterval.hpp"
#include "Utils/StrongType.hpp"

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

struct Config {
  ApiGateway::KlineInterval executionInterval{ApiGateway::KlineInterval::OneMinute};
  ApiGateway::AssetHistoryLength entryChannelLength{};
  ApiGateway::AssetHistoryLength exitChannelLength{};
  ApiGateway::AssetSymbol quoteAsset{};
  ApiGateway::AssetSymbols baseAssets{};
};

inline bool isValid(const Config& config) {
  const bool areLengthsValid = config.entryChannelLength.val_ > 0 && config.exitChannelLength.val_ > 0;

  return areLengthsValid && !config.baseAssets.empty();
}

}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy
