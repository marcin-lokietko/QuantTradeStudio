#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/AssetQuantities.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, ApiGateway::AssetQuantities& assets);

}