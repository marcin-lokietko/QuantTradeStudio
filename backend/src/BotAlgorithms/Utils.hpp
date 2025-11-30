#pragma once

#include <spdlog/spdlog.h>

#include <ranges>
#include <set>
#include <string>

#include "MarketService/IMarketService.hpp"

namespace BotAlgorithms {

template <std::ranges::range R>
  requires std::same_as<std::ranges::range_value_t<R>, ApiGateway::AssetSymbol>
void cancelOpenOrders(const MarketService::IMarketService& marketService, ApiGateway::AssetSymbol quoteAsset,
                      R&& baseAssets) {
  const auto openOrders = marketService.getOpenOrders() |
                          std::views::transform([](const auto& singleOrder) { return singleOrder.assetPair; });
  std::set<ApiGateway::TradingPairSymbol> openOrdersSymbols{openOrders.begin(), openOrders.end()};

  for (auto&& singleBaseAsset : baseAssets) {
    const ApiGateway::TradingPairSymbol symbol{std::move(singleBaseAsset), quoteAsset};

    if (openOrdersSymbols.count(symbol)) {
      SPDLOG_INFO("Cancelling orders on symbol={}", toString(symbol));
      marketService.cancelAllOrdersOnASymbol(symbol);
    }
  }
}

}  // namespace BotAlgorithms