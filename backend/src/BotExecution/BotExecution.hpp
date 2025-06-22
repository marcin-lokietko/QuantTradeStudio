#pragma once

#include <thread>
#include <vector>

#include "BotAlgorithms/ConfigExtractor.hpp"
#include "BotAlgorithms/Rebalancer/Rebalancer.hpp"
#include "IBotExecution.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/SystemTime.hpp"

namespace BotExecution {

class BotExecution : public IBotExecution {
 public:
  BotExecution(const MarketService::IMarketService& marketService) : marketService_(marketService) {}

  ApiGateway::StartBotResult startBot(const ApiGateway::BotConfig& botConfig) override;

  virtual ApiGateway::StopAllBotsResult stopAllBots() override;

 private:
  const MarketService::IMarketService& marketService_;
  Time::SystemTime systemTime_;
  const BotAlgorithms::ConfigExtractor configExtractor_{};
  std::vector<std::jthread> runningBots_{};
};

}  // namespace BotExecution