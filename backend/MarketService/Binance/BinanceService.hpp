#pragma once

#include "MarketService/Binance/Encryption.hpp"
#include "MarketService/IMarketService.hpp"

namespace MarketService::Binance {

class BinanceService : public IMarketService {
 public:
  BinanceService(std::string keysDir) : encryption(std::move(keysDir)){};

  std::string getServerTime() override;

  std::string getPrice(const std::string& tradingPairSymbol) override;

  Prices getPrices(const std::vector<std::string>& tradingPairSymbols) const override;

  KlineSequence getKlines(const std::string& symbol, const std::string& interval) override;

  Wallet getWallet() const override;

  void makeOrder(const std::string& symbol, const std::string& quantity, const std::string& price) override;

 private:
  std::string getAccountUrl() const;
  std::string getOrderUrl(const std::string& queryString) const;

  const std::string keysDir;
  const Encryption encryption;

  const std::string binanceTestnetBaseUrl = "https://testnet.binance.vision";
  const std::string binanceRealBaseUrl = "https://api.binance.com";
};

}  // namespace MarketService::Binance