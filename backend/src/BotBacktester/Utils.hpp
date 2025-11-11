#pragma once

#include <map>

#include "ApiGateway/TradingPairSymbol.hpp"
#include "MarketService/KlineSequence.hpp"

namespace BotBacktester {

bool doKlinesHaveDifferentSizesOrTimeRanges(
    const std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence>& klines);

}  // namespace BotBacktester