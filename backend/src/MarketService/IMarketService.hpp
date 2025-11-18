#pragma once

#include <optional>
#include <string>

#include "ApiGateway/AssetQuantities.hpp"
#include "ApiGateway/AssetQuantity.hpp"
#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/AssetValues.hpp"
#include "ApiGateway/OrderResult.hpp"
#include "ApiGateway/Orders.hpp"
#include "ApiGateway/Price.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "AssetPrices.hpp"
#include "AssetValues.hpp"
#include "Time.hpp"
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
  virtual Time getServerTime() = 0;
  virtual ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) const = 0;
  virtual AssetPrices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& symbols) const = 0;

  virtual ApiGateway::AssetQuantities getOwnedAssetsQuantity() const = 0;
  virtual ApiGateway::AssetValues getOwnedAssetsQuantityAndValue() const = 0;
  virtual AssetValues getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const = 0;

  virtual ApiGateway::OrderResult makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                            const ApiGateway::OrderSide& orderSide,
                                            const ApiGateway::AssetQuantity& quantity,
                                            const std::optional<ApiGateway::Price>& price) const = 0;
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