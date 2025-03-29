#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "MarketId.hpp"

namespace StockMarketService {

struct WalletItem {
  std::string asset{};
  std::string amountFree{};
  MarketId marketId{MarketId::Unknown};
};

using Wallet = std::vector<WalletItem>;

}  // namespace StockMarketService