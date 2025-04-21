#pragma once

#include <thread>
#include <vector>

#include "ConfigExtractor.hpp"
#include "IBotExecution.hpp"
#include "MarketService/IMarketService.hpp"
#include "Rebalancer/Rebalancer.hpp"
#include "Wallet/IWallet.hpp"

namespace BotExecution {

class BotExecution : public IBotExecution {
 public:
  BotExecution(const MarketService::IMarketService& marketService, const Wallet::IWallet& wallet)
      : marketService_(marketService), wallet_(wallet) {}

  ApiGateway::StartBotResult startBot(const ApiGateway::BotConfig& botConfig) override;

 private:
  void stopAllBots();

  const MarketService::IMarketService& marketService_;
  const Wallet::IWallet& wallet_;
  const ConfigExtractor configExtractor_{};
  std::vector<std::jthread> runningBots_{};
};

}  // namespace BotExecution