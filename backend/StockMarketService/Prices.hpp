#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Utils/StrongType.hpp"

namespace StockMarketService {

DEFINE_STRONG_TYPE(AssetPair, std::string);
DEFINE_STRONG_TYPE(Price, std::string);

struct SinglePrice {
  AssetPair assetPair{};
  Price price{};
};

using Prices = std::vector<SinglePrice>;

}  // namespace StockMarketService