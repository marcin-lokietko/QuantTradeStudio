#pragma once

#include <format>
#include <vector>

#include "ApiGateway/TradingPairSymbol.hpp"

namespace MarketService {

// Do not rename to TradingPairSymbols, intended to contain other trading info in the future
using TradingPairs = std::vector<ApiGateway::TradingPairSymbol>;

}  // namespace MarketService