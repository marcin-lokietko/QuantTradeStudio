#pragma once

#include <string>

#include "Config/ConfigParams.hpp"

namespace MarketService::Binance {

class IEncryption {
 public:
  virtual std::string generateSignature(const std::string& data) const = 0;
  virtual std::string getApiKey() const = 0;
};

}  // namespace MarketService::Binance