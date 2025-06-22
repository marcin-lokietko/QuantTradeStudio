#pragma once

#include <cstdint>
#include <format>
#include <map>
#include <string>
#include <vector>

#include "ApiGateway/Price.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

struct SingleAssetPrice {
  ApiGateway::TradingPairSymbol assetPair{};
  ApiGateway::Price price{};

  bool operator==(const SingleAssetPrice& other) const = default;
};

inline std::string toString(const SingleAssetPrice& singleAssetPrice) {
  return std::format("{{assetPair={}, price={}}}", toString(singleAssetPrice.assetPair), singleAssetPrice.price.val_);
}

using AssetPrices = std::vector<SingleAssetPrice>;
using AssetPricesMap = std::map<ApiGateway::TradingPairSymbol, ApiGateway::Price>;

inline AssetPricesMap asMap(const AssetPrices& assetPrices) {
  AssetPricesMap map;
  for (const auto& singlePrice : assetPrices) {
    map[singlePrice.assetPair] = singlePrice.price;
  }
  return map;
}

}  // namespace MarketService