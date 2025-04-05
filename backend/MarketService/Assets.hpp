#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "AssetSymbol.hpp"
#include "MarketId.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

DEFINE_STRONG_TYPE(AssetQuantity, std::string);

struct SingleAsset {
  AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  MarketId marketId{MarketId::Unknown};
};

using Assets = std::vector<SingleAsset>;

}  // namespace MarketService