#pragma once

#include <string>

#include "AssetSymbol.hpp"
#include "Assets.hpp"
#include "KlineSequence.hpp"
#include "Prices.hpp"
#include "TradingPairs.hpp"

namespace MarketService {

class IMarketService {
 public:
  virtual ~IMarketService() = default;
  virtual std::string getServerTime() = 0;
  virtual std::string getPrice(const std::string& symbol) = 0;
  virtual Prices getPrices(const std::vector<MarketService::TradingPairSymbol>& symbols) const = 0;
  virtual KlineSequence getKlines(const std::string& symbol, const std::string& interval) = 0;
  virtual Assets getAssets() const = 0;
  virtual void makeOrder(const std::string& symbol, const std::string& quantity, const std::string& price) = 0;
  virtual TradingPairs getTradingPairs(const AssetSymbol& quoteAsset) const = 0;
};

}  // namespace MarketService