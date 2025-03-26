#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace StockMarketService {

struct SinglePrice {
  std::string symbol{};
  std::string price{};
};

using Prices = std::vector<SinglePrice>;

}  // namespace StockMarketService