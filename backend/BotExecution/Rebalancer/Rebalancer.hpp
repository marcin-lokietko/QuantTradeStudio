#pragma once

#include <thread>

#include "Config.hpp"

namespace BotExecution::Rebalancer {

class Rebalancer {
 public:
  Rebalancer(Config&& config);

  void run(std::stop_token st);

 private:
  void rebalance();

  const Config config_;
};

}  // namespace BotExecution::Rebalancer