#pragma once

#include "ApiGateway/BotConfig.hpp"
#include "ApiGateway/StartBotResult.hpp"
#include "ApiGateway/StopAllBotsResult.hpp"

namespace BotExecution {

class IBotExecution {
 public:
  virtual ~IBotExecution() = default;

  virtual ApiGateway::StartBotResult startBot(const ApiGateway::BotConfig& botConfig) = 0;

  virtual ApiGateway::StopAllBotsResult stopAllBots() = 0;
};

}  // namespace BotExecution