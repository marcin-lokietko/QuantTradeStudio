#include <spdlog/spdlog.h>

#include <variant>

#include "BotAlgorithms/Rebalancer/Rebalancer.hpp"
#include "BotBacktester.hpp"

namespace BotBacktester {

void BotBacktester::testBot(const ApiGateway::BotConfig& botConfig,
                            const ApiGateway::BacktesterConfig& backtesterConfig) {
  stopAllBots();
  simulationEndPromise_ = std::promise<void>();
  backtesterConfig_ = backtesterConfig;

  systemTimeSimulator_ = std::make_unique<Simulators::SystemTimeSimulator>(backtesterConfig_.simulationStart,
                                                                           backtesterConfig_.simulationEnd);
  // 4. accept input parameters from FE and send evaluation result to FE
  // 5. Manual, UT, CT, E2E test everything and fix all the bugs
  // 6. Refactor everything
  // 7. add support for paging in getKlines (for large>1k time ranges)

  systemTimeSimulator_->registerSimulationEndCallback([this]() {
    // Note, that stopAllBots() cannot be called directly from the callback, because the
    // callback is executed in the thread of the bot. Joioning a thread frcom the same
    // thread is a deadlock. Instead a promise is used to notify the main thread (i.e.
    // BotBacktester's thread) that the simulation has ended.

    SPDLOG_INFO("Simulation end reached. Notifying main thread.");
    simulationEndPromise_.set_value();
  });

  auto extractedConfig = configExtractor_.getConfig(botConfig);

  std::visit(
      [this](auto&& config) {
        SPDLOG_INFO("Received bot configuration. Attempting to start the bot...");

        using T = std::decay_t<decltype(config)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          SPDLOG_ERROR("Invalid bot configuration");
        } else if constexpr (std::is_same_v<T, BotAlgorithms::Rebalancer::Config>) {
          SPDLOG_INFO("Received valid configuration for bot: Rebalancer");

          auto klineSequenceMap = buildKlineSequenceMap(config, this->backtesterConfig_.simulationStart,
                                                        this->backtesterConfig_.simulationEnd);

          this->marketServiceSimulator_ = std::make_unique<Simulators::MarketServiceSimulator>(
              klineSequenceMap, *this->systemTimeSimulator_, this->backtesterConfig_.transactionFeePercent,
              this->backtesterConfig_.initialOwnedAssets);

          this->evaluator_ = std::make_unique<Evaluator::Evaluator>(std::move(klineSequenceMap));

          this->runningBot_ =
              std::make_unique<std::jthread>([conf = std::move(config), &marketService = this->marketServiceSimulator_,
                                              &systemTime = systemTimeSimulator_](std::stop_token st) mutable {
                BotAlgorithms::Rebalancer::Rebalancer bot(std::move(conf), *marketService, *systemTime);
                bot.run(std::move(st));
              });
        }
      },
      extractedConfig);

  SPDLOG_INFO("Waiting for the simulation to end...");
  simulationEndPromise_.get_future().wait();
  SPDLOG_INFO("Simulation ended. Stopping all bots now...");
  stopAllBots();

  auto ownedAssetsHistory = marketServiceSimulator_->getOwnedAssetsHistory();
  auto backtestResults = this->evaluator_->evaluate(std::move(ownedAssetsHistory));
}

void BotBacktester::stopAllBots() {
  if (runningBot_) {
    runningBot_->request_stop();
  }
  SPDLOG_INFO("Running bot requested to stop. Waiting for bot to finish");
  if (runningBot_) {
    runningBot_->join();
  }

  SPDLOG_INFO("Bot stopped");
  runningBot_ = nullptr;
}

std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> BotBacktester::buildKlineSequenceMap(
    const BotAlgorithms::Rebalancer::Config& config, std::chrono::system_clock::time_point simStart,
    std::chrono::system_clock::time_point simEnd) {
  std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klineSequenceMap;

  for (const auto& singleBaseAssetShare : config.baseAssetShares) {
    const ApiGateway::TradingPairSymbol tradingPairSymbol{singleBaseAssetShare.assetSymbol, config.quoteAsset};

    const auto klines = historicalMarketDataProvider_.getKlines(
        tradingPairSymbol, MarketService::KlineInterval::OneMinute, simStart, simEnd);
    klineSequenceMap[tradingPairSymbol] = klines;
  }

  return klineSequenceMap;
}

}  // namespace BotBacktester