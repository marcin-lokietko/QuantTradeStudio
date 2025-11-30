#pragma once

#include "ApiGateway/BotConfig.hpp"
#include "Utils/StrongType.hpp"

namespace BotAlgorithms::MovingAverageCrossover {

struct Config {
  ApiGateway::ExecutionPeriod executionPeriod{};
  ApiGateway::AssetHistoryLength shortTermMovingAverageLength{};
  ApiGateway::AssetHistoryLength longTermMovingAverageLength{};
  ApiGateway::AssetSymbol quoteAsset{};
  ApiGateway::AssetSymbols baseAssets{};
};

inline bool isValid(const Config& config) {
  const bool areLengthsValid = config.shortTermMovingAverageLength.val_ > 0 &&
                               config.shortTermMovingAverageLength.val_ < config.longTermMovingAverageLength.val_;

  return config.executionPeriod.val_ > 0 && areLengthsValid && !config.baseAssets.empty();
}

}  // namespace BotAlgorithms::MovingAverageCrossover
