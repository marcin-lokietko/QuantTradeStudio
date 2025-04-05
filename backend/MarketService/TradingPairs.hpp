#pragma once

#include <vector>

#include "ApiGateway/TradingPairSymbol.hpp"
#include "AssetSymbol.hpp"

namespace MarketService {

struct SingleTradingPair {
  ApiGateway::TradingPairSymbol symbol{};
  AssetSymbol baseAsset{};
  AssetSymbol quoteAsset{};
};

using TradingPairs = std::vector<SingleTradingPair>;

}  // namespace MarketService