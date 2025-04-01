#pragma once

#include "Assets.hpp"

namespace Wallet {

class IWallet {
 public:
  virtual Assets getAssets() const = 0;
};

}  // namespace Wallet