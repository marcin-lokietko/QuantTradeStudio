#include <glog/logging.h>

#include <variant>

#include "BotExecution.hpp"

namespace BotExecution {

ApiGateway::StartBotResult BotExecution::startBot(const ApiGateway::BotConfig& botConfig) {
  auto extractedConfig = configExtractor_.getConfig(botConfig);
  auto startBotResult = ApiGateway::StartBotResult::Failure;

  stopAllBots();

  std::visit(
      [&runningBots_ = runningBots_, &startBotResult = startBotResult](auto&& config) {
        using T = std::decay_t<decltype(config)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          LOG(ERROR) << "Invalid bot configuration";
        } else if constexpr (std::is_same_v<T, Rebalancer::Config>) {
          LOG(INFO) << "Received valid configuration for bot: Rebalancer";
          runningBots_.emplace_back([conf = std::move(config)](std::stop_token st) mutable {
            Rebalancer::Rebalancer bot(std::move(conf));
            bot.run(std::move(st));
          });

          startBotResult = ApiGateway::StartBotResult::Success;
        }
      },
      extractedConfig);

  return startBotResult;
}

void BotExecution::stopAllBots() {
  for (auto& workerThread : runningBots_) {
    workerThread.request_stop();
  }
  for (auto& workerThread : runningBots_) {
    workerThread.join();
  }
}

}  // namespace BotExecution