#pragma once

#include <string>

namespace StockMarketService::Binance {

class Encryption {
 public:
  Encryption(std::string keysDir) : keysDir(std::move(keysDir)){};

  std::string generateSignature(const std::string& data) const;
  std::string getApiKey() const;

 private:
  std::string getSecretKey() const;
  const std::string keysDir;
};

}  // namespace StockMarketService::Binance