#pragma once

#include "ApiGateway/AssetSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace ApiGateway {

class TradingPairSymbol;
std::string toString(const TradingPairSymbol&);

struct TradingPairSymbol {
  TradingPairSymbol() = default;
  TradingPairSymbol(AssetSymbol base, AssetSymbol quote) : baseAsset(std::move(base)), quoteAsset(std::move(quote)) {}

  bool operator==(const TradingPairSymbol& other) const = default;
  bool operator<(const TradingPairSymbol& other) const { return toString(*this) < toString(other); }

  AssetSymbol baseAsset{};
  AssetSymbol quoteAsset{};
};

inline std::string toString(const TradingPairSymbol& tradingPairSymbol) {
  return tradingPairSymbol.baseAsset.val_ + tradingPairSymbol.quoteAsset.val_;
}

}  // namespace ApiGateway