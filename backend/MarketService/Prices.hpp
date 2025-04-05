#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ApiGateway/TradingPairSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

DEFINE_STRONG_TYPE(Price, std::string);

struct SinglePrice {
  ApiGateway::TradingPairSymbol assetPair{};
  Price price{};
};

using Prices = std::vector<SinglePrice>;

}  // namespace MarketService