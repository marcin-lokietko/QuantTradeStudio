#pragma once

#include <nlohmann/json.hpp>

#include "StockMarketService/Wallet.hpp"

namespace StockMarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, StockMarketService::Wallet& wallet);

}