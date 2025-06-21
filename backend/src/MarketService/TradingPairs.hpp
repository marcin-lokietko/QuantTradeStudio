#pragma once

#include <format>
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

inline std::string toString(const SingleTradingPair& singleTradingPair) {
  return std::format("{{symbol={}, baseAsset={}, quoteAsset={}}}", singleTradingPair.symbol.val_,
                     singleTradingPair.baseAsset.val_, singleTradingPair.quoteAsset.val_);
}

using TradingPairs = std::vector<SingleTradingPair>;

}  // namespace MarketService