#pragma once

#include <string>

namespace StockMarketService {

class IStockMarketService {
 public:
  virtual ~IStockMarketService() = default;
  virtual std::string getServerTime() = 0;
  virtual std::string getPrice(const std::string& symbol) = 0;
  virtual std::string getKlines(const std::string& symbol, const std::string& interval) = 0;
  virtual std::string getAccountData() = 0;
  virtual void makeOrder() = 0;
};

}  // namespace StockMarketService