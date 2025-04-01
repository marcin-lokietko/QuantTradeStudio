#pragma once

#include <vector>

#include "AssetSymbol.hpp"
#include "TradingPairSymbol.hpp"

namespace MarketService {

struct SingleTradingPair {
  TradingPairSymbol symbol{};
  AssetSymbol baseAsset{};
  AssetSymbol quoteAsset{};
};

using TradingPairs = std::vector<SingleTradingPair>;

}  // namespace MarketService