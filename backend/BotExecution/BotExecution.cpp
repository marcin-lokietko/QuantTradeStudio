#include <glog/logging.h>

#include <variant>

#include "BotExecution.hpp"

namespace BotExecution {

ApiGateway::StartBotResult BotExecution::startBot(const ApiGateway::BotConfig& botConfig) {
  const auto extractedConfig = configExtractor_.getConfig(botConfig);

  auto startBotResult = ApiGateway::StartBotResult::Failure;

  std::visit(
      [&runningBots_ = runningBots_, &rebalancer_ = rebalancer_, &startBotResult = startBotResult](auto& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          LOG(ERROR) << "Invalid bot configuration";
        } else if constexpr (std::is_same_v<T, Rebalancer::Config>) {
          LOG(INFO) << "Received valid configuration for bot: Rebalancer";
          runningBots_.emplace_back(&Rebalancer::Rebalancer::run, &rebalancer_, arg);

          startBotResult = ApiGateway::StartBotResult::Success;
        }
      },
      extractedConfig);

  return startBotResult;
}

}  // namespace BotExecution