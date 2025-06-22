#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ApiGateway/Price.hpp"
#include "Time.hpp"
#include "Utils/StrongType.hpp"

namespace MarketService {

struct Kline {
  std::chrono::system_clock::time_point openTime{};
  std::chrono::system_clock::time_point closeTime{};
  ApiGateway::Price openPrice{};
  ApiGateway::Price closePrice{};
  ApiGateway::Price lowPrice{};
  ApiGateway::Price highPrice{};

  bool operator==(const Kline& other) const = default;
};

inline std::string toString(const Kline& kline) {
  return std::format("{{openTime={}, closeTime={}, openPrice={}, closePrice={}, lowPrice={}, highPrice={}}}",
                     kline.openTime, kline.closeTime, kline.openPrice, kline.closePrice, kline.lowPrice,
                     kline.highPrice);
}

using KlineSequence = std::vector<Kline>;

}  // namespace MarketService