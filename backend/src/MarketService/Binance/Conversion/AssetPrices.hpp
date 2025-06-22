#pragma once

#include <nlohmann/json.hpp>

#include "MarketService/AssetPrices.hpp"
#include "MarketService/TradingPairSymbolDecoder.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::AssetPrices& assetPrices,
              const TradingPairSymbolDecoder& tradingPairSymbolDecoder);

}