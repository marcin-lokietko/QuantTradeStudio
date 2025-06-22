#pragma once

#include <cstdint>
#include <format>
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

}  // namespace ApiGateway