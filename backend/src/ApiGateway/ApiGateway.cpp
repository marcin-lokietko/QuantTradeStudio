#include <spdlog/spdlog.h>

#include <map>
#include <ranges>
#include <set>

#include "ApiGateway.hpp"
#include "MarketService/TradingPairs.hpp"
#include "Price.hpp"
#include "TradingPairSymbol.hpp"
#include "Utils/ToString.hpp"

namespace ApiGateway {

ApiGateway::ApiGateway(MarketService::IMarketService& marketService, BotExecution::IBotExecution& botExecution,
                       BotBacktester::IBotBacktester& botBacktester)
    : marketService_(marketService), botExecution_(botExecution), botBacktester_(botBacktester){};

AssetValues ApiGateway::getOwnedAssets() const {
  SPDLOG_INFO("getOwnedAssets called");

  const auto ownedAssets = marketService_.getOwnedAssetsQuantityAndValue();

  SPDLOG_INFO("getOwnedAssets result: ownedAssets={}", ::toString(ownedAssets));
  return ownedAssets;
}

OrderResult ApiGateway::makeOrder(const AssetSymbol& selectedBaseAsset, const AssetSymbol& selectedQuoteAsset,
                                  const OrderSide& orderSide, const AssetQuantity& baseAssetAmount) const {
  const TradingPairSymbol tradingPairSymbol{selectedBaseAsset, selectedQuoteAsset};

  SPDLOG_INFO("getOwnedAssets called: tradingPair={}; orderSide={}; baseAssetAmount={}", toString(tradingPairSymbol),
              toString(orderSide), baseAssetAmount);

  const auto price = marketService_.getPrice(tradingPairSymbol);
  const auto orderResult = marketService_.makeOrder(tradingPairSymbol, orderSide, baseAssetAmount, price);

  SPDLOG_INFO("getOwnedAssets result: orderResult={}", toString(orderResult));
  return orderResult;
}

Orders ApiGateway::getOpenOrders() const {
  SPDLOG_INFO("getOpenOrders called");

  const auto orders = marketService_.getOpenOrders();

  SPDLOG_INFO("getOpenOrders result: orders={}", ::toString(orders));
  return orders;
}

AvailableQuoteAssets ApiGateway::getAvailableQuoteAssets(const AssetSymbol& baseAsset) const {
  SPDLOG_INFO("getAvailableQuoteAssets called: baseAsset={}", baseAsset);

  const auto tradingPairs = marketService_.getTradingPairsWithBaseAsset(baseAsset);
  const auto tradingPairSymbolToPriceMap = asMap(marketService_.getPrices(tradingPairs));

  AvailableQuoteAssets availableQuoteAssets{};
  for (const auto& singleTradingPair : tradingPairs) {
    availableQuoteAssets.emplace_back(singleTradingPair.quoteAsset, tradingPairSymbolToPriceMap.at(singleTradingPair));
  }
  SPDLOG_INFO("getAvailableQuoteAssets result: availableQuoteAssets={}", ::toString(availableQuoteAssets));
  return availableQuoteAssets;
}

AssetSymbols ApiGateway::getAvailableBaseAssets(const std::optional<AssetSymbol>& quoteAsset) const {
  SPDLOG_INFO("getAvailableBaseAssets quoteAsset={}", quoteAsset);

  MarketService::TradingPairs tradingPairs{};
  if (quoteAsset) {
    tradingPairs = marketService_.getTradingPairsWithQuoteAsset(quoteAsset.value());
  } else {
    tradingPairs = marketService_.getAllTradingPairs();
  }

  std::set<AssetSymbol> baseAssets;
  for (const auto& singleTradingPair : tradingPairs) {
    baseAssets.insert(singleTradingPair.baseAsset);
  }

  const AssetSymbols assetSymbols{baseAssets.begin(), baseAssets.end()};
  SPDLOG_INFO("getAvailableBaseAssets result: assetSymbols={}", ::toString(assetSymbols));
  return assetSymbols;
}

AssetSymbols ApiGateway::getQuoteAssetsSuitableForRebalancing() const {
  SPDLOG_INFO("getQuoteAssetsSuitableForRebalancing called");
  MarketService::TradingPairs tradingPairs = marketService_.getAllTradingPairs();

  std::map<AssetSymbol, uint64_t> quoteAssetToNumOccurrences;
  for (const auto& singleTradingPair : tradingPairs) {
    ++quoteAssetToNumOccurrences[singleTradingPair.quoteAsset];
  }

  constexpr auto minOccurrencesForRebalancing = 2;
  AssetSymbols quoteAssetSymbols;
  for (const auto& [quoteAsset, numOccurrences] : quoteAssetToNumOccurrences) {
    if (numOccurrences >= minOccurrencesForRebalancing) {
      quoteAssetSymbols.push_back(quoteAsset);
    }
  }

  AssetSymbols suitableBaseAssets{quoteAssetSymbols.begin(), quoteAssetSymbols.end()};
  SPDLOG_INFO("getQuoteAssetsSuitableForRebalancing result: suitableBaseAssets={}", ::toString(suitableBaseAssets));
  return suitableBaseAssets;
}

StartBotResult ApiGateway::startBot(const BotConfig& botConfig) const {
  SPDLOG_INFO("startBot called: botConfig={}", toString(botConfig));

  const auto result = botExecution_.startBot(botConfig);

  SPDLOG_INFO("startBot result: result={}", toString(result));
  return result;
}

StopAllBotsResult ApiGateway::stopAllBots() const {
  SPDLOG_INFO("stopAllBots called");

  const auto result = botExecution_.stopAllBots();

  SPDLOG_INFO("stopAllBots result: result={}", toString(result));
  return result;
}

BacktestResults ApiGateway::testBot(const BotConfig& botConfig, const BacktestConfig& backtestConfig) const {
  return botBacktester_.testBot(botConfig, backtestConfig);
}

}  // namespace ApiGateway