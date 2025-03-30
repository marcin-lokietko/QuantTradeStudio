#pragma once

#include <nlohmann/json.hpp>

#include "StockMarketService/Prices.hpp"

namespace StockMarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, StockMarketService::Prices& prices);

}