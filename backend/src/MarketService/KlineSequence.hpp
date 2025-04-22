#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace MarketService {

struct Kline {
  uint64_t closeTime{};
  std::string closePrice{};
};

using KlineSequence = std::vector<Kline>;

}  // namespace MarketService