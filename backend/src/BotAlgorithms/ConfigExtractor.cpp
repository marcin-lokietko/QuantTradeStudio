#include "ConfigExtractor.hpp"

namespace BotAlgorithms {

ConfigExtractor::ExtractedConfig ConfigExtractor::getConfig(const ApiGateway::BotConfig& botConfig) const {
  if (botConfig.botName == ApiGateway::BotName{"Rebalancer"}) {
    const bool areMandatoryFieldsSet = botConfig.executionPeriod.has_value() &&
                                       botConfig.isExecutedImmediately.has_value() &&
                                       botConfig.quoteAsset.has_value() && botConfig.baseAssetShares.has_value();
    if (areMandatoryFieldsSet) {
      Rebalancer::Config config{botConfig.executionPeriod.value(), botConfig.isExecutedImmediately.value(),
                                botConfig.quoteAsset.value(), botConfig.baseAssetShares.value()};
      if (isValid(config)) {
        return config;
      }
    }
  } else if (botConfig.botName == ApiGateway::BotName{"MovingAverageCrossover"}) {
    const bool areMandatoryFieldsSet = botConfig.executionPeriod.has_value() &&
                                       botConfig.shortTermMovingAverageLength.has_value() &&
                                       botConfig.longTermMovingAverageLength.has_value() &&
                                       botConfig.quoteAsset.has_value() && botConfig.baseAssets.has_value();
    if (areMandatoryFieldsSet) {
      MovingAverageCrossover::Config config{
          .executionPeriod = botConfig.executionPeriod.value(),
          .shortTermMovingAverageLength = botConfig.shortTermMovingAverageLength.value(),
          .longTermMovingAverageLength = botConfig.longTermMovingAverageLength.value(),
          .quoteAsset = botConfig.quoteAsset.value(),
          .baseAssets = botConfig.baseAssets.value()};
      if (isValid(config)) {
        return config;
      }
    }
  }

  return {};
}

}  // namespace BotAlgorithms