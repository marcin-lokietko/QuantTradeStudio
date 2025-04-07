#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ApiGateway/Price.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

struct SingleAssetPrice {
  ApiGateway::TradingPairSymbol assetPair{};
  ApiGateway::Price price{};
};

using AssetPrices = std::vector<SingleAssetPrice>;

}  // namespace MarketService