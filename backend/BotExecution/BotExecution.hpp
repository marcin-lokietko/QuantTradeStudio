#pragma once

#include <thread>
#include <vector>

#include "ConfigExtractor.hpp"
#include "IBotExecution.hpp"
#include "MarketService/IMarketService.hpp"
#include "Rebalancer/Rebalancer.hpp"

namespace BotExecution {

class BotExecution : public IBotExecution {
 public:
  BotExecution(const MarketService::IMarketService& marketService) : marketService_(marketService) {}

  ApiGateway::StartBotResult startBot(const ApiGateway::BotConfig& botConfig) override;

 private:
  const MarketService::IMarketService& marketService_;
  const ConfigExtractor configExtractor_{};
  std::vector<std::jthread> runningBots_{};

  Rebalancer::Rebalancer rebalancer_{};
};

}  // namespace BotExecution