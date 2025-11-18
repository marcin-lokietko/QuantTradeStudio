#include "BotAlgorithms/MovingAverageCrossover/AssetHistory.hpp"

namespace BotAlgorithms::MovingAverageCrossover {

AssetHistory::AssetHistory(const ApiGateway::MovingAverageLength& shortTermMovingAverageLength,
                           const ApiGateway::MovingAverageLength& longTermMovingAverageLength)
    : shortTermAverage_(shortTermMovingAverageLength.val_), longTermAverage_(longTermMovingAverageLength.val_) {}

bool AssetHistory::isMinimalHistoryCollected() const {
  return shortTermAverage_.getAverage().has_value() && longTermAverage_.getAverage().has_value();
}

void AssetHistory::addValue(ApiGateway::Price price) {
  const double priceDouble = std::stod(price.val_);

  prevShortTermAverage_ = shortTermAverage_.getAverage();
  prevLongTermAverage_ = longTermAverage_.getAverage();

  shortTermAverage_.addValue(priceDouble);
  longTermAverage_.addValue(priceDouble);
}

bool AssetHistory::isSellSignalled() const {
  if (prevShortTermAverage_.has_value() && prevLongTermAverage_.has_value()) {
    return (prevShortTermAverage_.value() >= prevLongTermAverage_.value()) &&
           (shortTermAverage_.getAverage().value() < longTermAverage_.getAverage().value());
  }
  return false;
}

bool AssetHistory::isBuySignalled() const {
  if (prevShortTermAverage_.has_value() && prevLongTermAverage_.has_value()) {
    return (prevShortTermAverage_.value() <= prevLongTermAverage_.value()) &&
           (shortTermAverage_.getAverage().value() > longTermAverage_.getAverage().value());
  }
  return false;
}

}  // namespace BotAlgorithms::MovingAverageCrossover
