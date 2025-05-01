#pragma once

#include <gmock/gmock.h>

#include "MarketService/IMarketService.hpp"

namespace MarketService {

class MarketServiceMock : public IMarketService {
 public:
  MOCK_METHOD(std::string, getServerTime, (), (override));
  MOCK_METHOD(ApiGateway::Price, getPrice, (const ApiGateway::TradingPairSymbol& tradingPairSymbol), (override));
  MOCK_METHOD(AssetPrices, getPrices, (const std::vector<ApiGateway::TradingPairSymbol>& symbols), (const, override));
  MOCK_METHOD(KlineSequence, getKlines, (const std::string& symbol, const std::string& interval), (override));
  MOCK_METHOD(Assets, getOwnedAssets, (), (const, override));
  MOCK_METHOD(ApiGateway::OrderResult, makeOrder,
              (const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
               const ApiGateway::AssetQuantity& quantity, const ApiGateway::Price& price),
              (override));
  MOCK_METHOD(ApiGateway::OrderResult, makeMarketTypeOrderWithQuoteQuantity,
              (const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
               const ApiGateway::AssetQuantity& quoteQuantity),
              (const, override));
  MOCK_METHOD(TradingPairs, getAllTradingPairs, (), (const, override));
  MOCK_METHOD(TradingPairs, getTradingPairsWithQuoteAsset, (const ApiGateway::AssetSymbol& quoteAsset),
              (const, override));
  MOCK_METHOD(TradingPairs, getTradingPairsWithBaseAsset, (const ApiGateway::AssetSymbol& baseAsset),
              (const, override));
  MOCK_METHOD(ApiGateway::Orders, getOpenOrders, (), (const, override));
  MOCK_METHOD(ApiGateway::OrderResult, cancelAllOrdersOnASymbol, (const ApiGateway::TradingPairSymbol& symbol),
              (const, override));
};
}  // namespace MarketService