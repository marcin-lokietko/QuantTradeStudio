#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "MarketId.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

DEFINE_STRONG_TYPE(AssetSymbol, std::string);
DEFINE_STRONG_TYPE(FreeAssetAmount, std::string);

struct SingleAsset {
  AssetSymbol assetSymbol{};
  FreeAssetAmount freeAmount{};
  MarketId marketId{MarketId::Unknown};
};

using Assets = std::vector<SingleAsset>;

}  // namespace MarketService