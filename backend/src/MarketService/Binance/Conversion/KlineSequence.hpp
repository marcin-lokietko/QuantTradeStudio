#pragma once

#include <nlohmann/json.hpp>

#include "MarketService/KlineSequence.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::KlineSequence& klineSequence);

}