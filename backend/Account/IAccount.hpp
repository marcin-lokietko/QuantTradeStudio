#pragma once

#include "Wallet.hpp"

namespace Account {

class IAccount {
 public:
  virtual Wallet getWallet() const = 0;
};

}  // namespace Account