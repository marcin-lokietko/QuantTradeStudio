#pragma once

#include <nlohmann/json.hpp>

#include "MarketService/Wallet.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Wallet& wallet);

}