#pragma once

#include <string>

#include "KlineSequence.hpp"
#include "Prices.hpp"
#include "Wallet.hpp"

namespace MarketService {

class IMarketService {
 public:
  virtual ~IMarketService() = default;
  virtual std::string getServerTime() = 0;
  virtual std::string getPrice(const std::string& symbol) = 0;
  virtual Prices getPrices(const std::vector<std::string>& symbols) const = 0;
  virtual KlineSequence getKlines(const std::string& symbol, const std::string& interval) = 0;
  virtual Wallet getWallet() const = 0;
  virtual void makeOrder(const std::string& symbol, const std::string& quantity, const std::string& price) = 0;
};

}  // namespace MarketService