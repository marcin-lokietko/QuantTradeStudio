#pragma once

namespace ApiGateway {

enum class MarketId
{
  Unknown = 0,
  Binance = 1
};

inline std::string toString(const MarketId& marketId) {
  switch (marketId) {
    case MarketId::Unknown:
      return "Unknown";
    case MarketId::Binance:
      return "Binance";
    default:
      return "Invalid MarketId";
  }
}

}  // namespace ApiGateway
