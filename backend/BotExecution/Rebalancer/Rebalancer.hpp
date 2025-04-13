#pragma once

#include "Config.hpp"

namespace BotExecution::Rebalancer {

class Rebalancer {
 public:
  void run(const Config& config);
};

}  // namespace BotExecution::Rebalancer