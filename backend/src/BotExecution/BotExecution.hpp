#pragma once

#include <thread>
#include <vector>

#include "BotAlgorithms/ConfigExtractor.hpp"
#include "IBotExecution.hpp"
#include "MarketService/IHistoricalMarketDataProvider.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/SystemTime.hpp"

namespace BotExecution {

class BotExecution : public IBotExecution {
 public:
  BotExecution(const MarketService::IMarketService& marketService,
               const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider)
      : marketService_(marketService), historicalMarketDataProvider_(historicalMarketDataProvider) {}

  ApiGateway::StartBotResult startBot(const ApiGateway::BotConfig& botConfig) override;

  virtual ApiGateway::StopAllBotsResult stopAllBots() override;

 private:
  const MarketService::IMarketService& marketService_;
  const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider_;
  Time::SystemTime systemTime_;
  const BotAlgorithms::ConfigExtractor configExtractor_{};
  std::vector<std::jthread> runningBots_{};
};

}  // namespace BotExecution