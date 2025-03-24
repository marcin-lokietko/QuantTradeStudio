#pragma once

#include "StockMarketService/Binance/Encryption.h"
#include "StockMarketService/IStockMarketService.h"

namespace StockMarketService::Binance {

class BinanceService : public IStockMarketService {
 public:
  BinanceService(std::string keysDir) : encryption(std::move(keysDir)){};

  std::string getServerTime() override;

  std::string getPrice(const std::string& symbol) override;

  KlineSequence getKlines(const std::string& symbol, const std::string& interval) override;

  std::string getAccountData() const override;

  void makeOrder(const std::string& symbol, const std::string& quantity, const std::string& price) override;

 private:
  std::string getAccountUrl() const;
  std::string getOrderUrl(const std::string& queryString) const;

  const std::string keysDir;
  const Encryption encryption;

  const std::string binanceTestnetBaseUrl = "https://testnet.binance.vision";
  const std::string binanceRealBaseUrl = "https://api.binance.com";
};

}  // namespace StockMarketService::Binance