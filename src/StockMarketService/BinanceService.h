#pragma once

#include "IStockMarketService.h"

namespace StockMarketService {

class BinanceService : public IStockMarketService {
 public:
  BinanceService(std::string keysPath) : keysPath(std::move(keysPath)){};

  std::string getServerTime() override;

  std::string getPrice(const std::string& symbol) override;

  std::string getKlines(const std::string& symbol, const std::string& interval) override;

  std::string getAccountData() override;

  void makeOrder() override;

 private:
  const std::string keysPath;
};

}  // namespace StockMarketService