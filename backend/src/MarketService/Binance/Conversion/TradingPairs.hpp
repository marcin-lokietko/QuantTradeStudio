#pragma once

#include <nlohmann/json.hpp>

#include "MarketService/TradingPairs.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::TradingPairs& tradingPairs);

}