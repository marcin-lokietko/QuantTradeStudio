#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/Orders.hpp"
#include "MarketService/TradingPairSymbolDecoder.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, ApiGateway::Orders& orders,
              const TradingPairSymbolDecoder& tradingPairSymbolDecoder);

}