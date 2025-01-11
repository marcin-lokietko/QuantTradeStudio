#pragma once

#include <string>

#include "KlineSequence.h"

namespace StockMarketService {

struct Kline {
  uint64_t closeTime{};
  std::string closePrice{};
};

using KlineSequence = std::vector<Kline>;

}  // namespace StockMarketService