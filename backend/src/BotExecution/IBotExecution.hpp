#pragma once

#include "ApiGateway/BotConfig.hpp"
#include "ApiGateway/StartBotResult.hpp"

namespace BotExecution {

class IBotExecution {
 public:
  virtual ApiGateway::StartBotResult startBot(const ApiGateway::BotConfig& botConfig) = 0;
};

}  // namespace BotExecution