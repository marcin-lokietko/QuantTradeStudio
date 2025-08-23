#pragma once

#include "ApiGateway/BacktestConfig.hpp"
#include "ApiGateway/BacktestResults.hpp"
#include "ApiGateway/BotConfig.hpp"

namespace BotBacktester {

class IBotBacktester {
 public:
  virtual ~IBotBacktester() = default;

  virtual ApiGateway::BacktestResults testBot(const ApiGateway::BotConfig& botConfig,
                                              const ApiGateway::BacktestConfig& backtestConfig) = 0;
};

}  // namespace BotBacktester