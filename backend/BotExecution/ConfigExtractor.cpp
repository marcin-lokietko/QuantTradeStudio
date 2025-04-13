#include "ConfigExtractor.hpp"

namespace BotExecution {

ConfigExtractor::ExtractedConfig ConfigExtractor::getConfig(const ApiGateway::BotConfig& botConfig) const {
  if (botConfig.botName == ApiGateway::BotName{"Rebalancer"}) {
    const bool areMandatoryFieldsSet = botConfig.executionPeriod.has_value() &&
                                       botConfig.isExecutedImmediately.has_value() &&
                                       botConfig.quoteAsset.has_value() && botConfig.baseAssetShares.has_value();
    if (areMandatoryFieldsSet) {
      BotExecution::Rebalancer::Config config{botConfig.executionPeriod.value(),
                                              botConfig.isExecutedImmediately.value(), botConfig.quoteAsset.value(),
                                              botConfig.baseAssetShares.value()};
      if (isValid(config)) {
        return config;
      }
    }
  }

  return {};
}

}  // namespace BotExecution