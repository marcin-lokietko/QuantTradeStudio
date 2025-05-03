#pragma once

#include <string>

#include "Config/ConfigParams.hpp"

namespace MarketService::Binance {

class Encryption {
 public:
  Encryption(const Config::KeysCatalogPath& keysDir) : keysDir_(keysDir){};

  std::string generateSignature(const std::string& data) const;
  std::string getApiKey() const;

 private:
  std::string getSecretKey() const;
  const Config::KeysCatalogPath& keysDir_;
};

}  // namespace MarketService::Binance