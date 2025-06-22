#pragma once

#include "ApiGateway/BacktesterConfig.hpp"
#include "ApiGateway/BotConfig.hpp"

namespace BotBacktester {

class IBotBacktester {
 public:
  virtual ~IBotBacktester() = default;

  virtual void testBot(const ApiGateway::BotConfig& botConfig,
                       const ApiGateway::BacktesterConfig& backtesterConfig) = 0;
};

}  // namespace BotBacktester