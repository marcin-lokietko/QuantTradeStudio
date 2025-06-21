#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "MarketId.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

DEFINE_STRONG_TYPE(AssetQuantity, std::string);

struct SingleAsset {
  ApiGateway::AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  MarketId marketId{MarketId::Unknown};

  bool operator==(const SingleAsset& other) const = default;
};

inline std::string toString(const SingleAsset& singleAsset) {
  return std::format("{{assetSymbol={}, freeQuantity={}, marketId={}}}", singleAsset.assetSymbol.val_,
                     singleAsset.freeQuantity.val_, toString(singleAsset.marketId));
}

using Assets = std::vector<SingleAsset>;

}  // namespace MarketService