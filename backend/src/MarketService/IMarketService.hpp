#pragma once

#include <string>

#include "ApiGateway/AssetQuantity.hpp"
#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/OrderResult.hpp"
#include "ApiGateway/Orders.hpp"
#include "ApiGateway/Price.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "AssetPrices.hpp"
#include "Assets.hpp"
#include "KlineSequence.hpp"
#include "TradingPairs.hpp"

namespace MarketService {

enum MakeOrderResult
{
  Fail = 0,
  Success
};

class IMarketService {
 public:
  virtual ~IMarketService() = default;
  virtual std::string getServerTime() = 0;
  virtual ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) = 0;
  virtual AssetPrices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& symbols) const = 0;
  virtual KlineSequence getKlines(const std::string& symbol, const std::string& interval) = 0;
  virtual Assets getOwnedAssets() const = 0;

  virtual ApiGateway::OrderResult makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                            const ApiGateway::OrderSide& orderSide,
                                            const ApiGateway::AssetQuantity& quantity,
                                            const ApiGateway::Price& price) = 0;
  virtual ApiGateway::OrderResult makeMarketTypeOrderWithQuoteQuantity(
      const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
      const ApiGateway::AssetQuantity& quoteQuantity) const = 0;

  virtual TradingPairs getAllTradingPairs() const = 0;
  virtual TradingPairs getTradingPairsWithQuoteAsset(const ApiGateway::AssetSymbol& quoteAsset) const = 0;
  virtual TradingPairs getTradingPairsWithBaseAsset(const ApiGateway::AssetSymbol& baseAsset) const = 0;

  virtual ApiGateway::Orders getOpenOrders() const = 0;
  virtual ApiGateway::OrderResult cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const = 0;
};

}  // namespace MarketService