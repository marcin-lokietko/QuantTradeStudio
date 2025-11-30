#pragma once

#include <variant>

#include "ApiGateway/BotConfig.hpp"
#include "BotAlgorithms/DonchianChannelBreakoutStrategy/Config.hpp"
#include "BotAlgorithms/MovingAverageCrossover/Config.hpp"
#include "BotAlgorithms/Rebalancer/Config.hpp"

namespace BotAlgorithms {

class ConfigExtractor {
 public:
  using ExtractedConfig = std::variant<std::monostate, Rebalancer::Config, MovingAverageCrossover::Config,
                                       DonchianChannelBreakoutStrategy::Config>;
  ExtractedConfig getConfig(const ApiGateway::BotConfig& botConfig) const;
};

}  // namespace BotAlgorithms