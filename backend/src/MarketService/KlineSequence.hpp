#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ApiGateway/Price.hpp"
#include "Time.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

struct Kline {
  Time closeTime{};
  ApiGateway::Price closePrice{};

  bool operator==(const Kline& other) const = default;
};

inline std::string toString(const Kline& kline) {
  return std::format("{{closeTime={}, closePrice={}}}", kline.closeTime, kline.closePrice);
}

using KlineSequence = std::vector<Kline>;

}  // namespace MarketService