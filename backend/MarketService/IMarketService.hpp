#pragma once

#include <string>

#include "ApiGateway/AssetQuantity.hpp"
#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/Price.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "Assets.hpp"
#include "KlineSequence.hpp"
#include "Prices.hpp"
#include "TradingPairs.hpp"

namespace MarketService {

class IMarketService {
 public:
  virtual ~IMarketService() = default;
  virtual std::string getServerTime() = 0;
  virtual ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) = 0;
  virtual Prices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& symbols) const = 0;
  virtual KlineSequence getKlines(const std::string& symbol, const std::string& interval) = 0;
  virtual Assets getAssets() const = 0;
  virtual void makeOrder(const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::AssetQuantity& quantity,
                         const ApiGateway::Price& price) = 0;
  virtual TradingPairs getTradingPairs(const ApiGateway::AssetSymbol& quoteAsset) const = 0;
};

}  // namespace MarketService