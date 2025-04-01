#pragma once

#include "Wallet.hpp"

namespace ApiGateway {

class IApiGateway {
 public:
  virtual Wallet getWallet() const = 0;
};

}  // namespace ApiGateway