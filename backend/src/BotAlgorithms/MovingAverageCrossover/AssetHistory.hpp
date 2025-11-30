#pragma once

#include "ApiGateway/Price.hpp"
#include "BotAlgorithms/MovingAverageCrossover/AverageCalculator.hpp"
#include "Config.hpp"

namespace BotAlgorithms::MovingAverageCrossover {

class AssetHistory {
 public:
  AssetHistory(const ApiGateway::AssetHistoryLength& shortTermMovingAverageLength,
               const ApiGateway::AssetHistoryLength& longTermMovingAverageLength);

  bool isMinimalHistoryCollected() const;

  void addValue(ApiGateway::Price price);

  bool isSellSignalled() const;

  bool isBuySignalled() const;

 private:
  AverageCalculator<double> shortTermAverage_;
  AverageCalculator<double> longTermAverage_;

  std::optional<double> prevShortTermAverage_;
  std::optional<double> prevLongTermAverage_;
};

}  // namespace BotAlgorithms::MovingAverageCrossover
