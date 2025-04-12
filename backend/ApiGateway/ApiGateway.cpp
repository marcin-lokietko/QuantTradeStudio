#include <glog/logging.h>

#include <ranges>
#include <set>

#include "ApiGateway.hpp"
#include "MarketService/TradingPairs.hpp"
#include "Price.hpp"
#include "TradingPairSymbol.hpp"

namespace ApiGateway {

ApiGateway::ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet)
    : marketService_(marketService), wallet_(wallet){};

Assets ApiGateway::getAssets() const { return wallet_.getAssets(); }

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
  MarketService::TradingPairs tradingPairs{};
  if (quoteAsset) {
    tradingPairs = marketService_.getTradingPairsWithQuoteAsset(quoteAsset.value());
  }
  tradingPairs = marketService_.getAllTradingPairs();

  std::set<AssetSymbol> baseAssets;
  for (const auto& singleTradingPair : tradingPairs) {
    baseAssets.insert(singleTradingPair.baseAsset);
  }

  return {baseAssets.begin(), baseAssets.end()};
}

}  // namespace ApiGateway