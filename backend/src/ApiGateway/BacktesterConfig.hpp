#pragma once

#include <format>

#include "ApiGateway/AssetQuantities.hpp"
#include "Utils/StrongType.hpp"
#include "Utils/ToString.hpp"

namespace ApiGateway {

DEFINE_STRONG_TYPE(TransactionFeePercent, double);

struct BacktesterConfig {
  TransactionFeePercent transactionFeePercent{};
  AssetQuantities initialOwnedAssets{};
  std::chrono::system_clock::time_point simulationStart{};
  std::chrono::system_clock::time_point simulationEnd{};

  bool operator==(const BacktesterConfig& other) const = default;
};

inline std::string toString(const BacktesterConfig& backtesterConfig) {
  return std::format("{{transactionFeePercent={}, initialOwnedAssets={}, simulationStart={}, simulationEnd={}}}",
                     backtesterConfig.transactionFeePercent, ::toString(backtesterConfig.initialOwnedAssets),
                     backtesterConfig.simulationStart, backtesterConfig.simulationEnd);
}
}  // namespace ApiGateway
