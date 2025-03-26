#pragma once

#include "Wallet.h"

namespace Account {

class IAccount {
 public:
  virtual Wallet getWallet() const = 0;
};

}  // namespace Account