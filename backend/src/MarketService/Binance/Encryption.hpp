#pragma once

#include "MarketService/Binance/IEncryption.hpp"

namespace MarketService::Binance {

class Encryption : public IEncryption {
 public:
  Encryption(const Config::KeysCatalogPath& keysDir) : keysDir_(keysDir){};

  std::string generateSignature(const std::string& data) const override;
  std::string getApiKey() const override;

 private:
  std::string getSecretKey() const;
  const Config::KeysCatalogPath& keysDir_;
};

}  // namespace MarketService::Binance