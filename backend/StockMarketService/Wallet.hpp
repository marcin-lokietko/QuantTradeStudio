#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "MarketId.hpp"
#include "Utils/StrongType.hpp"

namespace StockMarketService {

DEFINE_STRONG_TYPE(Asset, std::string);
DEFINE_STRONG_TYPE(FreeAssetAmount, std::string);

struct WalletItem {
  Asset asset{};
  FreeAssetAmount amountFree{};
  MarketId marketId{MarketId::Unknown};
};

using Wallet = std::vector<WalletItem>;

}  // namespace StockMarketService