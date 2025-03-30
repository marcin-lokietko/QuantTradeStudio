#pragma once

#include <nlohmann/json.hpp>

#include "MarketService/Prices.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Prices& prices);

}