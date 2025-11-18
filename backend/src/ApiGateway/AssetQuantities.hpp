#pragma once

#include <cstdint>
#include <format>
#include <map>
#include <string>
#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "MarketId.hpp"
#include "Utils/StrongType.hpp"

namespace ApiGateway {

DEFINE_STRONG_TYPE(AssetQuantity, std::string);

struct SingleAssetQuantity {
  AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  MarketId marketId{MarketId::Unknown};

  bool operator==(const SingleAssetQuantity& other) const = default;
};

inline std::string toString(const SingleAssetQuantity& singleAsset) {
  return std::format("{{assetSymbol={}, freeQuantity={}, marketId={}}}", singleAsset.assetSymbol.val_,
                     singleAsset.freeQuantity.val_, toString(singleAsset.marketId));
}

using AssetQuantities = std::vector<SingleAssetQuantity>;
using AssetQuantitiesMap = std::map<ApiGateway::AssetSymbol, ApiGateway::AssetQuantity>;

inline AssetQuantitiesMap asMap(const AssetQuantities& assetQuantities) {
  AssetQuantitiesMap map;
  for (const auto& singleQuantity : assetQuantities) {
    map[singleQuantity.assetSymbol] = singleQuantity.freeQuantity;
  }
  return map;
}

}  // namespace ApiGateway