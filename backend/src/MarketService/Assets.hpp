#pragma once

#include <cstdint>
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

using Assets = std::vector<SingleAsset>;

}  // namespace MarketService