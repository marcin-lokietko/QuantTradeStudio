#include "ApiGateway.hpp"

#include <glog/logging.h>

#include <map>
#include <ranges>
#include <set>

#include "MarketService/TradingPairs.hpp"
#include "Price.hpp"
#include "TradingPairSymbol.hpp"

namespace ApiGateway {

ApiGateway::ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet,
                       BotExecution::IBotExecution& botExecution)
    : marketService_(marketService), wallet_(wallet), botExecution_(botExecution){};

Assets ApiGateway::getOwnedAssets() const { return wallet_.getOwnedAssets(); }

OrderResult ApiGateway::makeOrder(const AssetSymbol& selectedBaseAsset, const AssetSymbol& selectedQuoteAsset,
                                  const OrderSide& orderSide, const AssetQuantity& baseAssetAmount) const {
  TradingPairSymbol tradingPairSymbol{selectedBaseAsset.val_ + selectedQuoteAsset.val_};

  const auto price = marketService_.getPrice(tradingPairSymbol);

  return marketService_.makeOrder(tradingPairSymbol, orderSide, baseAssetAmount, price);
}

Orders ApiGateway::getOpenOrders() const { return marketService_.getOpenOrders(); }

AvailableQuoteAssets ApiGateway::getAvailableQuoteAssets(const AssetSymbol& baseAsset) const {
  const auto tradingPairs = marketService_.getTradingPairsWithBaseAsset(baseAsset);

  std::vector<TradingPairSymbol> tradingPairSymbols;
  for (const auto& singleTradingPair : tradingPairs) {
    tradingPairSymbols.push_back(singleTradingPair.symbol);
  }
  const auto tradingPairSymbolToPriceMap = asMap(marketService_.getPrices(tradingPairSymbols));

  AvailableQuoteAssets availableQuoteAssets{};
  for (const auto& singleTradingPair : tradingPairs) {
    availableQuoteAssets.emplace_back(singleTradingPair.quoteAsset,
                                      tradingPairSymbolToPriceMap.at(singleTradingPair.symbol));
  }
  return availableQuoteAssets;
}

AssetSymbols ApiGateway::getAvailableBaseAssets(const std::optional<AssetSymbol>& quoteAsset) const {
  const auto argumentAsStr = quoteAsset ? quoteAsset.value().val_ : "nullopt";
  LOG(INFO) << "getAvailableBaseAssets quoteAsset=" << argumentAsStr;
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

  return {baseAssets.begin(), baseAssets.end()};
}

AssetSymbols ApiGateway::getQuoteAssetsSuitableForRebalancing() const {
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

  return {quoteAssetSymbols.begin(), quoteAssetSymbols.end()};
}

StartBotResult ApiGateway::startBot(const BotConfig& botConfig) const { return botExecution_.startBot(botConfig); }

StopAllBotsResult ApiGateway::stopAllBots() const { return botExecution_.stopAllBots(); }

}  // namespace ApiGateway