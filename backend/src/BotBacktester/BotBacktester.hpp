#pragma once

#include <future>
#include <memory>
#include <thread>

#include "BotAlgorithms/ConfigExtractor.hpp"
#include "BotBacktester/Evaluator/Evaluator.hpp"
#include "BotBacktester/Simulators/MarketServiceSimulator.hpp"
#include "BotBacktester/Simulators/SystemTimeSimulator.hpp"
#include "IBotBacktester.hpp"
#include "MarketService/IHistoricalMarketDataProvider.hpp"

namespace BotBacktester {

class BotBacktester : public IBotBacktester {
 public:
  BotBacktester(const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider)
      : historicalMarketDataProvider_(historicalMarketDataProvider) {}

  ApiGateway::BacktestResults testBot(const ApiGateway::BotConfig& botConfig,
                                      const ApiGateway::BacktestConfig& backtestConfig) override;

 private:
  void stopAllBots();

  std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> buildKlineSequenceMap(
      const BotAlgorithms::Rebalancer::Config& config, std::chrono::system_clock::time_point simStart,
      std::chrono::system_clock::time_point simEnd);

  std::unique_ptr<Simulators::SystemTimeSimulator> systemTimeSimulator_;
  const BotAlgorithms::ConfigExtractor configExtractor_{};
  const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider_;
  std::unique_ptr<Simulators::MarketServiceSimulator> marketServiceSimulator_;
  std::unique_ptr<std::jthread> runningBot_{};
  std::unique_ptr<Evaluator::Evaluator> evaluator_{};

  ApiGateway::BacktestConfig backtestConfig_{};
  std::promise<void> simulationEndPromise_;
};

}  // namespace BotBacktester