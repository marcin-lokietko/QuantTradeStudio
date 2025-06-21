#include "BotExecution.hpp"

#include <spdlog/spdlog.h>

#include <memory>
#include <variant>

#include "Utils/Time/Time.hpp"

namespace BotExecution {

ApiGateway::StartBotResult BotExecution::startBot(const ApiGateway::BotConfig& botConfig) {
  auto extractedConfig = configExtractor_.getConfig(botConfig);
  auto startBotResult = ApiGateway::StartBotResult::Failure;

  stopAllBots();

  std::visit(
      [&runningBots_ = runningBots_, &marketService_ = marketService_, &startBotResult = startBotResult,
       &wallet_ = wallet_](auto&& config) {
        SPDLOG_INFO("Received bot configuration. Attempting to start the bot...");

        using T = std::decay_t<decltype(config)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          SPDLOG_ERROR("Invalid bot configuration");
        } else if constexpr (std::is_same_v<T, Rebalancer::Config>) {
          SPDLOG_INFO("Received valid configuration for bot: Rebalancer");

          runningBots_.emplace_back([conf = std::move(config), &marketService_ = marketService_,
                                     &wallet_ = wallet_](std::stop_token st) mutable {
            Rebalancer::Rebalancer bot(std::move(conf), marketService_, wallet_, std::make_unique<Time::Time>());
            bot.run(std::move(st));
          });

          startBotResult = ApiGateway::StartBotResult::Success;
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