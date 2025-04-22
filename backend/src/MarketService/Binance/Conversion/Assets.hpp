#pragma once

#include <nlohmann/json.hpp>

#include "MarketService/Assets.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Assets& assets);

}