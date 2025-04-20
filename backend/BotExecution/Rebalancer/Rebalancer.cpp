#include <thread>

#include "Rebalancer.hpp"
#include "glog/logging.h"

namespace BotExecution::Rebalancer {

Rebalancer::Rebalancer(Config&& config) : config_(std::move(config)) {}

void Rebalancer::run(std::stop_token st) {
  LOG(INFO) << "Rebalancer started execution";
  const std::chrono::seconds executionPeriod(config_.executionPeriod.val_);

  if (!st.stop_requested() && config_.isExecutedImmediately.val_) {
    rebalance();
  }
  while (!st.stop_requested()) {
    std::this_thread::sleep_for(executionPeriod);
    if (!st.stop_requested()) {
      rebalance();
    }
  }
  LOG(INFO) << "Rebalancer stopped execution";
}

void Rebalancer::rebalance() { LOG(INFO) << "Rebalancer executes now!"; }

}  // namespace BotExecution::Rebalancer