#pragma once

#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"

namespace MarketService {

struct SingleTradingPair {
  ApiGateway::TradingPairSymbol symbol{};
  ApiGateway::AssetSymbol baseAsset{};
  ApiGateway::AssetSymbol quoteAsset{};
};

using TradingPairs = std::vector<SingleTradingPair>;

}  // namespace MarketService