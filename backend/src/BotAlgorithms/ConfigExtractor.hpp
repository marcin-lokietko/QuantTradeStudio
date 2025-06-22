#pragma once

#include <variant>

#include "ApiGateway/BotConfig.hpp"
#include "BotAlgorithms/Rebalancer/Config.hpp"

namespace BotAlgorithms {

class ConfigExtractor {
 public:
  using ExtractedConfig = std::variant<std::monostate, Rebalancer::Config>;

  ExtractedConfig getConfig(const ApiGateway::BotConfig& botConfig) const;
};

}  // namespace BotAlgorithms