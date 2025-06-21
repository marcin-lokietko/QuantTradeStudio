#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace MarketService {

struct Kline {
  uint64_t closeTime{};
  std::string closePrice{};

  bool operator==(const Kline& other) const = default;
};

inline std::string toString(const Kline& kline) {
  return std::format("{{closeTime={}, closePrice={}}}", kline.closeTime, kline.closePrice);
}

using KlineSequence = std::vector<Kline>;

}  // namespace MarketService