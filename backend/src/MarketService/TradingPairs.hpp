#pragma once

#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"

namespace MarketService {

struct SingleTradingPair {
  ApiGateway::TradingPairSymbol symbol{};
  ApiGateway::AssetSymbol baseAsset{};
  ApiGateway::AssetSymbol quoteAsset{};

  bool operator==(const SingleTradingPair& other) const = default;
};

using TradingPairs = std::vector<SingleTradingPair>;

}  // namespace MarketService