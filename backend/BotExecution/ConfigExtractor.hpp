#pragma once

#include <variant>

#include "ApiGateway/BotConfig.hpp"
#include "BotExecution/Rebalancer/Config.hpp"

namespace BotExecution {

class ConfigExtractor {
 public:
  using ExtractedConfig = std::variant<std::monostate, BotExecution::Rebalancer::Config>;

  ExtractedConfig getConfig(const ApiGateway::BotConfig& botConfig) const;
};

}  // namespace BotExecution