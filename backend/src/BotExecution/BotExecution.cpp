#include "BotExecution.hpp"

#include <spdlog/spdlog.h>

#include <memory>
#include <variant>

#include "BotAlgorithms/DonchianChannelBreakoutStrategy/DonchianChannelBreakoutStrategy.hpp"
#include "BotAlgorithms/MovingAverageCrossover/MovingAverageCrossover.hpp"
#include "BotAlgorithms/Rebalancer/Rebalancer.hpp"
#include "Utils/Time/SystemTime.hpp"

namespace BotExecution {

ApiGateway::StartBotResult BotExecution::startBot(const ApiGateway::BotConfig& botConfig) {
  auto extractedConfig = configExtractor_.getConfig(botConfig);
  auto startBotResult = ApiGateway::StartBotResult::Failure;

  stopAllBots();

  std::visit(
      [&runningBots_ = runningBots_, &marketService_ = marketService_,
       &historicalMarketDataProvider_ = historicalMarketDataProvider_, &startBotResult = startBotResult,
       &systemTime = systemTime_](auto&& config) {
        SPDLOG_INFO("Received bot configuration. Attempting to start the bot...");

        using T = std::decay_t<decltype(config)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          SPDLOG_ERROR("Invalid bot configuration");
        } else if constexpr (std::is_same_v<T, BotAlgorithms::Rebalancer::Config>) {
          SPDLOG_INFO("Received valid configuration for bot: Rebalancer");

          runningBots_.emplace_back([conf = std::move(config), &marketService_ = marketService_,
                                     &systemTime = systemTime](std::stop_token st) mutable {
            BotAlgorithms::Rebalancer::Rebalancer bot(std::move(conf), marketService_, systemTime);
            bot.run(std::move(st));
          });

          startBotResult = ApiGateway::StartBotResult::Success;
        } else if constexpr (std::is_same_v<T, BotAlgorithms::MovingAverageCrossover::Config>) {
          SPDLOG_INFO("Received valid configuration for bot: MovingAverageCrossover");

          runningBots_.emplace_back([conf = std::move(config), &marketService_ = marketService_,
                                     &systemTime = systemTime](std::stop_token st) mutable {
            BotAlgorithms::MovingAverageCrossover::MovingAverageCrossover bot(std::move(conf), marketService_,
                                                                              systemTime);
            bot.run(std::move(st));
          });

          startBotResult = ApiGateway::StartBotResult::Success;
        } else if constexpr (std::is_same_v<T, BotAlgorithms::DonchianChannelBreakoutStrategy::Config>) {
          SPDLOG_INFO("Received valid configuration for bot: DonchianChannelBreakoutStrategy");

          runningBots_.emplace_back([conf = std::move(config), &marketService_ = marketService_,
                                     &historicalMarketDataProvider_ = historicalMarketDataProvider_,
                                     &systemTime = systemTime](std::stop_token st) mutable {
            BotAlgorithms::DonchianChannelBreakoutStrategy::DonchianChannelBreakoutStrategy bot(
                std::move(conf), marketService_, historicalMarketDataProvider_, systemTime);
            bot.run(std::move(st));
          });

          startBotResult = ApiGateway::StartBotResult::Success;
        } else {
          SPDLOG_ERROR("Unsupported bot configuration type");
        }
      },
      extractedConfig);

  return startBotResult;
}

ApiGateway::StopAllBotsResult BotExecution::stopAllBots() {
  SPDLOG_INFO("Attempting to stop all running bots");

  for (auto& workerThread : runningBots_) {
    workerThread.request_stop();
  }
  SPDLOG_INFO("All bots requeste to stop. Waiting for all bots to finish");
  for (auto& workerThread : runningBots_) {
    workerThread.join();
  }

  SPDLOG_INFO("All bots stopped");
  runningBots_.clear();
  return ApiGateway::StopAllBotsResult::Success;
}

}  // namespace BotExecution