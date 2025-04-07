#pragma once

#include "ApiGateway/Assets.hpp"

namespace Wallet {

class IWallet {
 public:
  virtual ApiGateway::Assets getAssets() const = 0;
};

}  // namespace Wallet