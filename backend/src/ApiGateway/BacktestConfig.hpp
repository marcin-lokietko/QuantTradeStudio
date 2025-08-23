#pragma once

#include <format>

#include "ApiGateway/AssetQuantities.hpp"
#include "Utils/StrongType.hpp"
#include "Utils/ToString.hpp"

namespace ApiGateway {

DEFINE_STRONG_TYPE(TransactionFeePercent, double);

struct BacktestConfig {
  TransactionFeePercent transactionFeePercent{};
  AssetQuantities initialOwnedAssets{};
  std::chrono::system_clock::time_point simulationStart{};
  std::chrono::system_clock::time_point simulationEnd{};

  bool operator==(const BacktestConfig& other) const = default;
};

inline std::string toString(const BacktestConfig& backtestConfig) {
  return std::format("{{transactionFeePercent={}, initialOwnedAssets={}, simulationStart={}, simulationEnd={}}}",
                     backtestConfig.transactionFeePercent, ::toString(backtestConfig.initialOwnedAssets),
                     backtestConfig.simulationStart, backtestConfig.simulationEnd);
}
}  // namespace ApiGateway
