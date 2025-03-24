#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "KlineSequence.h"

namespace StockMarketService {

struct Kline {
  uint64_t closeTime{};
  std::string closePrice{};
};

using KlineSequence = std::vector<Kline>;

}  // namespace StockMarketService