#include <thread>

#include "Rebalancer.hpp"
#include "glog/logging.h"

namespace BotExecution::Rebalancer {

void Rebalancer::run(const Config& config) {
  LOG(ERROR) << "Rebalancer started execution";
  const std::chrono::seconds executionPeriod(config.executionPeriod.val_);
  while (true) {
    std::this_thread::sleep_for(executionPeriod);
    LOG(ERROR) << "Rebalancer executes now!";
  }
}
}  // namespace BotExecution::Rebalancer